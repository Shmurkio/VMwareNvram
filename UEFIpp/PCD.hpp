#pragma once

#include "UEFIpp.hpp"

constexpr EFI_GUID gPcdProtocolGuid = { 0x11B34006, 0xD85B, 0x4D0A, { 0xA2, 0x90, 0xD5, 0xA5, 0x71, 0x31, 0x0E, 0xF7 } };

using PcdProtocolSetSkuFn = VOID(__cdecl)(UINT64 SkuId);
using PcdProtocolGet8Fn = UINT8(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGet16Fn = UINT16(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGet32Fn = UINT32(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGet64Fn = UINT64(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGetPointerFn = PVOID(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGetBooleanFn = BOOLEAN(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGetSizeFn = UINT64(__cdecl)(UINT64 TokenNumber);
using PcdProtocolGetEx8Fn = UINT8(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetEx16Fn = UINT16(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetEx32Fn = UINT32(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetEx64Fn = UINT64(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetExPointerFn = PVOID(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetExBooleanFn = BOOLEAN(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolGetExSizeFn = UINT64(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber);
using PcdProtocolSet8Fn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, UINT8 Value);
using PcdProtocolSet16Fn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, UINT16 Value);
using PcdProtocolSet32Fn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, UINT32 Value);
using PcdProtocolSet64Fn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, UINT64 Value);
using PcdProtocolSetPointerFn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, PUINT64 SizeOfBuffer, PVOID Buffer);
using PcdProtocolSetBooleanFn = EFI_STATUS(__cdecl)(UINT64 TokenNumber, BOOLEAN Value);
using PcdProtocolSetEx8Fn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, UINT8 Value);
using PcdProtocolSetEx16Fn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, UINT16 Value);
using PcdProtocolSetEx32Fn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, UINT32 Value);
using PcdProtocolSetEx64Fn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, UINT64 Value);
using PcdProtocolSetExPointerFn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, PUINT64 SizeOfBuffer, PVOID Buffer);
using PcdProtocolSetExBooleanFn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, BOOLEAN Value);
using PcdProtocolCallbackFn = VOID(__cdecl)(PCEFI_GUID CallbackGuid, UINT64 CallbackToken, PVOID TokenData, UINT64 TokenDataSize);
using PcdProtocolCallbackOnsetFn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNUmber, PcdProtocolCallbackFn* CallBackFunction);
using PcdProtocolCancelCallbackFn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, UINT64 TokenNumber, PcdProtocolCallbackFn* CallBackFunction);
using PcdProtocolGetNextTokenFn = EFI_STATUS(__cdecl)(PCEFI_GUID Guid, PUINT64 TokenNUmber);
using PcdProtocolGetNextTokenSpaceFn = EFI_STATUS(__cdecl)(PCEFI_GUID* Guid);

typedef struct _PCD_PROTOCOL
{
	PcdProtocolSetSkuFn* SetSku;

	PcdProtocolGet8Fn* Get8;
	PcdProtocolGet16Fn* Get16;
	PcdProtocolGet32Fn* Get32;
	PcdProtocolGet64Fn* Get64;
	PcdProtocolGetPointerFn* GetPtr;
	PcdProtocolGetBooleanFn* GetBool;
	PcdProtocolGetSizeFn* GetSize;

	PcdProtocolGetEx8Fn* Get8Ex;
	PcdProtocolGetEx16Fn* Get16Ex;
	PcdProtocolGetEx32Fn* Get32Ex;
	PcdProtocolGetEx64Fn* Get64Ex;
	PcdProtocolGetExPointerFn* GetPtrEx;
	PcdProtocolGetExBooleanFn* GetBoolEx;
	PcdProtocolGetExSizeFn* GetSizeEx;

	PcdProtocolSet8Fn* Set8;
	PcdProtocolSet16Fn* Set16;
	PcdProtocolSet32Fn* Set32;
	PcdProtocolSet64Fn* Set64;
	PcdProtocolSetPointerFn* SetPtr;
	PcdProtocolSetBooleanFn* SetBool;

	PcdProtocolSetEx8Fn* Set8Ex;
	PcdProtocolSetEx16Fn* Set16Ex;
	PcdProtocolSetEx32Fn* Set32Ex;
	PcdProtocolSetEx64Fn* Set64Ex;
	PcdProtocolSetExPointerFn* SetPtrEx;
	PcdProtocolSetExBooleanFn* SetBoolEx;

	PcdProtocolCallbackOnsetFn* CallbackOnSet;
	PcdProtocolCancelCallbackFn* CancelCallback;
	PcdProtocolGetNextTokenFn* GetNextToken;
	PcdProtocolGetNextTokenSpaceFn* GetNextTokenSpace;
} PCD_PROTOCOL, *PPCD_PROTOCOL;

using CPCD_PROTOCOL = const PCD_PROTOCOL;
using PCPCD_PROTOCOL = const PCD_PROTOCOL*;