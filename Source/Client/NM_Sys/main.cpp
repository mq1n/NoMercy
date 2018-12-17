#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#include "Kernel-Bridge/DriverEvents.h"
#include "Kernel-Bridge/FilterCallbacks.h"
#include "Kernel-Bridge/IOCTLHandlers.h"
#include "Kernel-Bridge/IOCTLs.h"

#include "API/CppSupport.h"
#include <WdkTypes.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

namespace 
{
	PCWSTR DeviceNamePath = L"\\Device\\NoMercy";
	PCWSTR DeviceLinkPath = L"\\??\\NoMercy";
	PDEVICE_OBJECT DeviceInstance = NULL;
	PFLT_FILTER FilterHandle = NULL;
}

extern "C"
{
	DRIVER_INITIALIZE DriverEntry;

	static NTSTATUS SEC_ENTRY FilterInstanceSetup(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_SETUP_FLAGS Flags, _In_ DEVICE_TYPE VolumeDeviceType, _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType);

	static NTSTATUS SEC_ENTRY FilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags);

	static NTSTATUS OnUnsupportedCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

	_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
	static NTSTATUS DriverControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

	_Dispatch_type_(IRP_MJ_CREATE)
	_Dispatch_type_(IRP_MJ_CLOSE)
	_Dispatch_type_(IRP_MJ_CLEANUP)
	static NTSTATUS DriverStub(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

	static NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT DriverObject);
}

#ifdef ALLOC_PRAGMA
	#pragma alloc_text(INIT, DriverEntry)
	#pragma alloc_text(PAGE, FilterUnload)
	#pragma alloc_text(PAGE, FilterInstanceSetup)
	#pragma alloc_text(PAGE, DriverStub)
	#pragma alloc_text(PAGE, DriverUnload)
#endif


NTSTATUS CallIoctlDispatcher(IN PIOCTL_INFO RequestInfo, OUT PSIZE_T ResponseLength)
{
	ULONG ExceptionCode = 0;
	PEXCEPTION_POINTERS ExceptionPointers = NULL;

	NTSTATUS ntStatus;
	__try 
	{
		ntStatus = DispatchIOCTL(RequestInfo, ResponseLength);
	}
	__except (ExceptionCode = GetExceptionCode(), ExceptionPointers = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER)
	{
		ntStatus = ExceptionCode;
        SysErrorLog("[NoMercy]: Exception catched in IOCTL handler!\nCode: 0x%X\tAddress: 0x%p\tCTL: 0x%X",
            ExceptionCode, ExceptionPointers->ExceptionRecord->ExceptionAddress, RequestInfo->ControlCode
        );
	}
	return ntStatus;
}


// IOCTLs handler:
static NTSTATUS OnUnsupportedCall(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
static NTSTATUS DriverControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	IOCTL_INFO RequestInfo;
	RequestInfo.ControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;

	switch (EXTRACT_CTL_METHOD(RequestInfo.ControlCode))
	{
		case METHOD_BUFFERED: 
		{
			RequestInfo.InputBuffer			= Irp->AssociatedIrp.SystemBuffer;
			RequestInfo.OutputBuffer		= Irp->AssociatedIrp.SystemBuffer;
			RequestInfo.InputBufferSize		= IrpStack->Parameters.DeviceIoControl.InputBufferLength;
			RequestInfo.OutputBufferSize	= IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			Irp->IoStatus.Status			= CallIoctlDispatcher(&RequestInfo, &Irp->IoStatus.Information);
			break;
		}
		case METHOD_NEITHER: 
		{
			RequestInfo.InputBuffer			= IrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
			RequestInfo.OutputBuffer		= Irp->UserBuffer;
			RequestInfo.InputBufferSize		= IrpStack->Parameters.DeviceIoControl.InputBufferLength;
			RequestInfo.OutputBufferSize	= IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			Irp->IoStatus.Status			= CallIoctlDispatcher(&RequestInfo, &Irp->IoStatus.Information);
			break;
		}
		case METHOD_IN_DIRECT:
		case METHOD_OUT_DIRECT: 
		{
			RequestInfo.InputBuffer			= Irp->AssociatedIrp.SystemBuffer;
			RequestInfo.OutputBuffer		= Irp->MdlAddress ? MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority) : NULL;
			RequestInfo.InputBufferSize		= IrpStack->Parameters.DeviceIoControl.InputBufferLength;
			RequestInfo.OutputBufferSize	= IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			Irp->IoStatus.Status			= CallIoctlDispatcher(&RequestInfo, &Irp->IoStatus.Information);
			break;
		}
		default: 
		{
			Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
			SysErrorLog("[NoMercy]: Unknown method of IRP!\n");
		}
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}

_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
_Dispatch_type_(IRP_MJ_CLEANUP)
static NTSTATUS DriverStub(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	PAGED_CODE();

	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	switch (IrpStack->MajorFunction) 
	{
		case IRP_MJ_CREATE:
			OnDriverCreate(DeviceObject, FilterHandle, Irp, IrpStack);
			break;
		case IRP_MJ_CLEANUP:
			OnDriverCleanup(DeviceObject, FilterHandle, Irp, IrpStack);
			break;
		case IRP_MJ_CLOSE:
			OnDriverClose(DeviceObject, FilterHandle, Irp, IrpStack);
			break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// Operations registration:
static CONST FLT_OPERATION_REGISTRATION FilterCallbacks[] =
{
	{
		IRP_MJ_CREATE,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_READ,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_WRITE,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_DEVICE_CONTROL,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_INTERNAL_DEVICE_CONTROL,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_FILE_SYSTEM_CONTROL,
		0,
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
		0, 
		reinterpret_cast<PFLT_PRE_OPERATION_CALLBACK>(FilterPreOperation),
		reinterpret_cast<PFLT_POST_OPERATION_CALLBACK>(FilterPostOperation)
	},
	{
		IRP_MJ_OPERATION_END
	}
};

// What we want to filter:
static CONST FLT_REGISTRATION FilterRegistration = 
{
	sizeof(FLT_REGISTRATION),	// Size
	FLT_REGISTRATION_VERSION,	// Version
	0,							// Flags
	NULL,						// ContextRegistration
	FilterCallbacks,			// Operation callbacks
	reinterpret_cast<PFLT_FILTER_UNLOAD_CALLBACK>(FilterUnload), // MiniFilterUnload
	FilterInstanceSetup, // InstanceSetup
	NULL, // InstanceQueryTeardown
	NULL, // InstanceTeardownStart
	NULL, // InstanceTeardownComplete
	NULL, // GenerateFileName
	NULL, // GenerateDestinationFileName
	NULL, // NormalizeNameComponent
	NULL, // TransactionNotifierCallback
	NULL, // NormalizeNameComponentExCallback
};

static NTSTATUS SEC_ENTRY FilterInstanceSetup(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_SETUP_FLAGS Flags, _In_ DEVICE_TYPE VolumeDeviceType, _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	PAGED_CODE();

	return VolumeDeviceType == FILE_DEVICE_NETWORK_FILE_SYSTEM ? STATUS_FLT_DO_NOT_ATTACH : STATUS_SUCCESS;
}

static NTSTATUS SEC_ENTRY FilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags) 
{
	PAGED_CODE();

	OnFilterUnload(DeviceInstance, FilterHandle, Flags);
	
	if (FilterHandle) 
		FltUnregisterFilter(FilterHandle);

	return STATUS_SUCCESS;
}

// Driver entry routine
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) 
{
	// Process params
	UNREFERENCED_PARAMETER(RegistryPath);

	if (!DriverObject)
	{
		SysErrorLog("[NoMercy]: Driver can not loaded!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	// Initialization of POOL_NX_OPTIN:
	ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

	// Global objects initialization
	__crt_init();

	// Hello world!
	SysInfoLog("[NoMercy]: Driver entry init has been started, system range start in %p, Our entry at: %p\n", MmSystemRangeStart, DriverEntry);

	// Register unload routine
	DriverObject->DriverUnload = reinterpret_cast<PDRIVER_UNLOAD>(DriverUnload);

	// Register driver IRP callbacks
	for (ULONG t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		DriverObject->MajorFunction[t] = OnUnsupportedCall;

	DriverObject->MajorFunction[IRP_MJ_CREATE]			= DriverStub;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP]			= DriverStub;
	DriverObject->MajorFunction[IRP_MJ_CLOSE]			= DriverStub;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]	= DriverControl;

	SysInfoLog("[NoMercy]: Unload and IRP callbacks succesfully created!\n");

	// Veriable decleration
	NTSTATUS ntStatus = 0;

	// Normalize name and symbolic link.
	UNICODE_STRING deviceName, deviceLink;
	RtlInitUnicodeString(&deviceName, DeviceNamePath);
	RtlInitUnicodeString(&deviceLink, DeviceLinkPath);

	// Create the device.
	ntStatus = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceInstance);
	if (!NT_SUCCESS(ntStatus))
	{
		SysErrorLog("[NoMercy]: IoCreateDevice fail! Ntstatus: %X\n", ntStatus);
		return ntStatus;
	}
	SysInfoLog("[NoMercy]: Device succesfully created! Ctx: %p\n", DeviceInstance);

	// Create the symbolic link
	ntStatus = IoCreateSymbolicLink(&deviceLink, &deviceName);
	if (!NT_SUCCESS(ntStatus))
	{
		SysErrorLog("[NoMercy]: IoCreateSymbolicLink fail! Ntstatus: %X\n", ntStatus);
		return ntStatus;
	}
	SysInfoLog("[NoMercy]: Symbolic link succesfully created!\n");

	// Register minifilter
	ntStatus = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);
	if (!NT_SUCCESS(ntStatus))
	{
		SysErrorLog("[NoMercy]: FltRegisterFilter fail! Ntstatus: %X\n", ntStatus);
		return ntStatus;
	}
	SysInfoLog("[NoMercy]: Minifilter succesfully registered! Handle: %p\n", FilterHandle);

	// Start minifilter as filter
	ntStatus = FltStartFiltering(FilterHandle);
	if (!NT_SUCCESS(ntStatus))
	{
		SysErrorLog("[NoMercy]: FltStartFiltering fail! Ntstatus: %X\n", ntStatus);
		FltUnregisterFilter(FilterHandle);
		FilterHandle = NULL;
		return ntStatus;
	}
	SysInfoLog("[NoMercy]: Minifilter succesfully started!\n");

	// TODO: çalýþan nm_service yoksa driverý unload et (return unsuccess)

	// Custom load callback
	OnDriverLoad(DriverObject, DeviceInstance, FilterHandle, RegistryPath);

	SysInfoLog("[NoMercy]: Successfully loaded!\n");
	return STATUS_SUCCESS;
}

// Driver unload routine
static NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	PAGED_CODE();

	// Custom unload callback
	OnDriverUnload(DriverObject, DeviceInstance);

	// Global objects destroying
	__crt_deinit(); 

	// Normalize symbolic link
	UNICODE_STRING DeviceLink;
	RtlInitUnicodeString(&DeviceLink, DeviceLinkPath);

	// Delete symbolic link object
	IoDeleteSymbolicLink(&DeviceLink);

	// Delete device object
	if (DeviceInstance)
	{
		IoDeleteDevice(DeviceInstance);
	}

	SysInfoLog("[NoMercy]: Successfully unloaded!\n");
	return STATUS_SUCCESS;
}

