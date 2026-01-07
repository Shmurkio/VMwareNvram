#include "VMware.hpp"
#include "PCD.hpp"

static PPCD_PROTOCOL gPcd = nullptr;

static
EFI_STATUS
GetPcd(
	OUT PPCD_PROTOCOL& Pcd
)
{
	Pcd = nullptr;

	if (gPcd)
	{
		Pcd = gPcd;
		return EFI_SUCCESS;
	}

	EFI_STATUS Status = gBS->LocateProtocol(const_cast<PEFI_GUID>(&gPcdProtocolGuid), nullptr, reinterpret_cast<PVOID*>(&gPcd));

	if (EfiError(Status))
	{
		return Status;
	}

	Pcd = gPcd;

	return EFI_SUCCESS;
}

EFI_STATUS
VMware::GetRegion(
	OUT PCTRL_REGISTERS& Ctrl,
	OUT PCMD_BUFFER& Cmd
)
{
	Ctrl = nullptr;
	Cmd = nullptr;

	PPCD_PROTOCOL Pcd = nullptr;
	EFI_STATUS Status = GetPcd(Pcd);

	if (EfiError(Status))
	{
		return Status;
	}

	UINT32 Base = Pcd->Get32(PCD_NVRAM_REGION_TOKEN);

	if (!Base)
	{
		return EFI_DEVICE_ERROR;
	}

	Ctrl = reinterpret_cast<PCTRL_REGISTERS>(Base + VMware::NVRAM_REGION_CTRL_REGS_OFFSET);
	Cmd = reinterpret_cast<PCMD_BUFFER>(Base + Ctrl->BufferOffset);

	return EFI_SUCCESS;
}