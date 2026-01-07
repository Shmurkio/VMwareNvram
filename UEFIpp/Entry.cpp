#include "UEFIpp.hpp"
#include "VMware.hpp"

using GetFlagFn = UINT8(__fastcall)(
	VOID
);

GetFlagFn* GetFlag = nullptr;

static constexpr EFI_STATUS gEfiStatusFromCode[10] =
{
	EFI_SUCCESS,
	EFI_OUT_OF_RESOURCES,
	EFI_NOT_FOUND,
	EFI_BUFFER_TOO_SMALL,
	EFI_BAD_BUFFER_SIZE,
	EFI_INVALID_PARAMETER,
	EFI_WRITE_PROTECTED,
	EFI_INVALID_PARAMETER,
	EFI_SECURITY_VIOLATION,
	EFI_UNSUPPORTED
};

static
VOID
DumpHex(
	IN PCUINT8 Data,
	IN UINT64 Length,
	IN UINT64 BytesPerLine = 16
)
{
	if (!Data || !Length || !BytesPerLine)
	{
		return;
	}

	for (UINT64 i = 0; i < Length; i += BytesPerLine)
	{
		UINT64 LineLen = (Length - i < BytesPerLine) ? (Length - i) : BytesPerLine;

		Serial::Out << "    0x" << Serial::Hex << Data + i << Serial::Dec << ": ";

		for (UINT64 j = 0; j < BytesPerLine; ++j)
		{
			if (j < LineLen)
			{
				UINT8 B = Data[i + j];

				if (B < 0x10)
				{
					Serial::Out << "0";
				}

				Serial::Out << Serial::Hex << B << Serial::Dec;
			}
			else
			{
				Serial::Out << "  ";
			}

			if (j + 1 != BytesPerLine)
			{
				Serial::Out << " ";
			}
		}

		Serial::Out << "  |";

		for (UINT64 j = 0; j < LineLen; ++j)
		{
			UINT8 B = Data[i + j];

			if (B >= 0x20 && B <= 0x7E)
			{
				Serial::Out << static_cast<CHAR>(B);
			}
			else
			{
				Serial::Out << '.';
			}
		}

		for (UINT64 j = LineLen; j < BytesPerLine; ++j)
		{
			Serial::Out << ' ';
		}

		Serial::Out << "|" << Serial::Endl;
	}
}

static
inline
UINT64
StrLenBytes(
	IN PCWSTR Str
)
{
	UINT64 Len = 0;

	while (Str[Len])
	{
		++Len;
	}

	return 2 * (Len + 1);
}

EFI_STATUS
GetVariableRaw(
	IN PWSTR VariableName,
	IN PEFI_GUID VendorGuid,
	OUT PUINT32 Attributes,
	OUT PUINT64 DataSize,
	OUT PVOID Data
)
{
	// Validate parameters.
	if (!VariableName || !VendorGuid || !DataSize)
	{
		return EFI_INVALID_PARAMETER;
	}

	// Get NVRAM variable name length.
	UINT32 VariableNameLength = StrLenBytes(VariableName);

	if (!VariableNameLength)
	{
		return EFI_INVALID_PARAMETER;
	}

	// Get mailbox regions.
	VMware::PCTRL_REGISTERS Ctrl = nullptr;
	VMware::PCMD_BUFFER Cmd = nullptr;
	EFI_STATUS Status = VMware::GetRegion(Ctrl, Cmd);

	if (EfiError(Status))
	{
		return Status;
	}

	// Get request size.
	UINT32 RequestSize = sizeof(VMware::CMD_GET_VAR_REQ) + VariableNameLength;

	// Set TPL_NOTIFY.
	gBS->RaiseTPL(TPL_NOTIFY);

	// Set the request size in the control register.
	Ctrl->Size = RequestSize;

	// Populate mailbox request.
	VMware::PCMD_GET_VAR_REQ Request = &Cmd->GetVariableRequest;
	Util::CopyMemory(VendorGuid, &Request->VendorGuid, sizeof(EFI_GUID), false);
	Request->DataSize = *DataSize;
	Request->VariableNameLength = VariableNameLength;
	Util::CopyMemory(VariableName, &Request->VariableName, VariableNameLength, false);

	// Get flag.
	UINT8 Flag = GetFlag();

	// Submit the command.
	Ctrl->Control = VMware::CMD_GET_VARIABLE | (Flag ? 0x100 : 0);

	// Map code to status.
	Status = EFI_INVALID_PARAMETER;

	if (Ctrl->Control < 10)
	{
		Status = gEfiStatusFromCode[Ctrl->Control];
	}

	// Process mailbox response depending on status.
	if (!EfiError(Status))
	{
		// Get mailbox response.
		VMware::PCMD_GET_VAR_RESP Response = &Cmd->GetVariableResponse;

		// Set result parameters.
		if (Attributes)
		{
			*Attributes = Response->Attributes;
		}

		*DataSize = Response->DataSize;
		Util::CopyMemory(&Response->Data, Data, Response->DataSize, false);
	}
	else if (Status == EFI_BUFFER_TOO_SMALL)
	{
		// Set if data size of the variable is queried.
		*DataSize = Ctrl->Size;
	}

	// Restore TPL_APPLICATION.
	gBS->RestoreTPL(TPL_APPLICATION);

	// Return status of the submitted command.
	return Status;
}

static
EFI_STATUS
TestGetVariableRaw(
	VOID
)
{
	EFI_GUID VendorGuid = { 0x12441234, 0x8132, 0x4852, { 0x90, 0xCC, 0x55, 0x1A, 0x4E, 0x4A, 0x7F, 0x1C } };
	PWSTR VariableName = const_cast<PWSTR>(L"TestVariable");

	UINT32 Attributes = 0;
	UINT64  DataSize = 0;

	EFI_STATUS Status = GetVariableRaw(VariableName, &VendorGuid, &Attributes, &DataSize, nullptr);

	if (Status != EFI_BUFFER_TOO_SMALL)
	{
		return Status;
	}

	PUINT8 Data = new UINT8[DataSize];

	if (!Data)
	{
		return EFI_OUT_OF_RESOURCES;
	}

	Status = GetVariableRaw(VariableName, &VendorGuid, &Attributes, &DataSize, Data);

	if (EfiError(Status))
	{
		return Status;
	}

	Serial::Out << "[i] Read:" << Serial::Endl
		<< "    - Status: 0x" << Serial::Hex << Status << Serial::Dec << Serial::Endl
		<< "    - Size: " << DataSize << Serial::Endl
		<< "    - Attributes: 0x" << Serial::Hex << Attributes << Serial::Dec << Serial::Endl;

	DumpHex(Data, DataSize, 16);

	delete[] Data;

	return Status;
}

static
EFI_STATUS
SetTestVariable(
	VOID
)
{
	EFI_GUID VendorGuid = { 0x12441234, 0x8132, 0x4852, { 0x90, 0xCC, 0x55, 0x1A, 0x4E, 0x4A, 0x7F, 0x1C } };
	PWSTR VariableName = const_cast<PWSTR>(L"TestVariable");

	CHAR Data[] = "Lorem ipsum dolor sit amet consectetur adipiscing elit. Pretium tellus duis convallis tempus leo eu aenean. Iaculis massa nisl malesuada lacinia integer nunc posuere. Conubia nostra inceptos himenaeos orci varius natoque penatibus. Nulla molestie mattis scelerisque maximus eget fermentum odio. Blandit quis suspendisse aliquet nisi sodales consequat magna. Ligula congue sollicitudin erat viverra ac tincidunt nam. Velit aliquam imperdiet mollis nullam volutpat porttitor ullamcorper. Dui felis venenatis ultrices proin libero feugiat tristique. Cubilia curae hac habitasse platea dictumst lorem ipsum. Sem placerat in id cursus mi pretium tellus. Fringilla lacus nec metus bibendum egestas iaculis massa. Taciti sociosqu ad litora torquent per conubia nostra. Ridiculus mus donec rhoncus eros lobortis nulla molestie. Mauris pharetra vestibulum fusce dictum risus blandit quis. Finibus facilisis dapibus etiam interdum tortor ligula congue. Justo lectus commodo augue arcu dignissim velit aliquam. Primis vulputate ornare sagittis vehicula praesent dui felis. Senectus netus suscipit auctor curabitur facilisi cubilia curae. Quisque faucibus ex sapien vitae pellentesque sem placerat.";
	UINT32 Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
	UINT64 DataSize = sizeof(Data);

	return gRT->SetVariable(VariableName, &VendorGuid, Attributes, DataSize, Data);
}

//
// Main.
//
EFI_STATUS
EfiMain(
	MAYBE_UNUSED IN PCSTR* ArgV,
	MAYBE_UNUSED IN UINT64 ArgC
)
{
	//
	// Get the NVRAM DXE driver.
	//
	PVOID GetVariable = gRT->GetVariable;
	PVOID ImageBase = nullptr;

	EFI_STATUS Status = Image::GetImageBase(GetVariable, ImageBase, 64);

	if (EfiError(Status))
	{
		Serial::Out << "[i] Failed to get NvramDxe.efi image base" << Serial::Endl;
		return Status;
	}

	UINT64 ImageSize = 0;
	Status = Image::GetImageSize(ImageBase, ImageSize);

	if (EfiError(Status))
	{
		Serial::Out << "[i] Failed to get NvramDxe.efi image size" << Serial::Endl;
		return Status;
	}

	Serial::Out << "[i] ImageBase: 0x" << Serial::Hex << ImageBase << Serial::Dec << Serial::Endl
		<< "[i] ImageSize: " << ImageSize << Serial::Endl;

	Status = Util::FindPattern(reinterpret_cast<UINT64>(ImageBase), ImageSize, "8A 05", GetFlag);

	if (EfiError(Status))
	{
		Serial::Out << "[i] Failed to find GetFlag" << Serial::Endl;
		return Status;
	}

	Serial::Out << "[i] GetFlag: 0x" << Serial::Hex << GetFlag << Serial::Dec << Serial::Endl;

	return TestGetVariableRaw();
}