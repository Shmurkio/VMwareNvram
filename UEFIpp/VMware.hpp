#pragma once

#include "UEFIpp.hpp"

#pragma pack(push, 1)
namespace VMware
{
	//
	// NVRAM communication buffer.
	// Base address: Pcd->Read32(17) = 0xFFC00000.
	// CTRL_REGISTERS: BaseAddress + 0x280 = 0xFFC00280.
	// CMD_BUFFER: BaseAddress + CMD_BUFFER.BufferOffset = 0xFFC00000 + 0x8000 = 0xFFC08000.
	//
	typedef struct _CTRL_REGISTERS
	{
		UINT32 Magic;				// "PVNv"
		UINT32 Control;				// IN: Command | Flags, OUT: Status
		UINT32 Size;				// IN: RequestSize, OUT: RequiredSize
		UINT32 BufferOffset;		// Command buffer offset (0x8000, relative to 0xFFC00000)
		UINT32 BufferCapacity;		// Command buffer capacity
	} CTRL_REGISTERS, *PCTRL_REGISTERS;

	using CCTRL_REGISTERS = const CTRL_REGISTERS;
	using PCCTRL_REGISTERS = const CTRL_REGISTERS*;

	typedef struct _CMD_GET_VAR_REQ
	{
		EFI_GUID VendorGuid;		// 0x0
		UINT64 DataSize;			// 0x10
		UINT32 VariableNameLength;	// 0x18
		WCHAR VariableName[];		// 0x1C
	} CMD_GET_VAR_REQ, *PCMD_GET_VAR_REQ;

	using CCMD_GET_VAR_REQ = const CMD_GET_VAR_REQ;
	using PCCMD_GET_VAR_REQ = const CMD_GET_VAR_REQ*;

	typedef struct _CMD_GET_VAR_RESP
	{
		UINT32 Attributes;			// 0x0
		UINT32 DataSize;			// 0x4
		UINT8 Data[];				// 0x8
	} CMD_GET_VAR_RESP, *PCMD_GET_VAR_RESP;

	using CCMD_GET_VAR_RESP = const CMD_GET_VAR_RESP;
	using PCCMD_GET_VAR_RESP = const CMD_GET_VAR_RESP*;

	typedef struct _CMD_BUFFER
	{
		union
		{
			CMD_GET_VAR_REQ  GetVariableRequest;
			CMD_GET_VAR_RESP GetVariableResponse;
		};
	} CMD_BUFFER, *PCMD_BUFFER;

	using CCMD_BUFFER = const CMD_BUFFER;
	using PCCMD_BUFFER = const CMD_BUFFER*;

	// PCD tokens.
	constexpr CUINT64 PCD_NVRAM_REGION_TOKEN = 17;

	// Region offsets.
	constexpr CUINT64 NVRAM_REGION_CTRL_REGS_OFFSET = 0x280;

	// Magic.
	constexpr CUINT32 NVRAM_CTRL_MAGIC = 'vNVP'; // "PVNv"

	// CMD codes.
	constexpr CUINT32 CMD_GET_VARIABLE = 209;

	EFI_STATUS
	GetRegion(
		OUT PCTRL_REGISTERS& Ctrl,
		OUT PCMD_BUFFER& Cmd
	);
}
#pragma pack(pop)