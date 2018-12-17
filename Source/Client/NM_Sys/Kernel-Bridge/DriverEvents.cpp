#include <fltKernel.h>
#include <WdkTypes.h>
#include "FilterCallbacks.h"

VOID OnDriverLoad(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT DeviceObject, PFLT_FILTER FilterHandle, PUNICODE_STRING RegistryPath) 
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	PEPROCESS pCurrProc = PsGetCurrentProcess();
	SysInfoLog("[NoMercy]: Driver initialize routine(%s) called! Process : %lld (%p)\n", __FUNCTION__, reinterpret_cast<UINT64>(PsGetCurrentProcessId()), pCurrProc);

	if (FilterHandle) 
	{
		// Start filter server communication
		NTSTATUS ntStatus = Communication::StartServer(FilterHandle);
		if (!NT_SUCCESS(ntStatus))
		{
			SysErrorLog("Filter communication server start fail! Status: 0x%X\n", ntStatus);
			return;
		}

		// Register Handle protection
		ntStatus = KbCallbacks::StartObHandlesFilter();
		if (!NT_SUCCESS(ntStatus))
		{
			SysErrorLog("OB Callback initilization fail! Status: 0x%X\n", ntStatus);
			return;
		}

		// Register notify callbacks
		ntStatus = KbCallbacks::StartPsProcessFilter();
		if (!NT_SUCCESS(ntStatus))
		{
			SysErrorLog("Process Callback initilization fail! Status: 0x%X\n", ntStatus);
			return;
		}

		ntStatus = KbCallbacks::StartPsThreadFilter();
		if (!NT_SUCCESS(ntStatus))
		{
			SysErrorLog("Thread Callback initilization fail! Status: 0x%X\n", ntStatus);
			return;
		}

		ntStatus = KbCallbacks::StartPsImageFilter();
		if (!NT_SUCCESS(ntStatus))
		{
			SysErrorLog("Image Callback initilization fail! Status: 0x%X\n", ntStatus);
			return;
		}

		SysInfoLog("[NoMercy]: Driver initialize completed!\n");
	}
}

VOID OnDriverUnload(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT DeviceObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(DeviceObject);
}

VOID OnFilterUnload(PDEVICE_OBJECT DeviceObject, PFLT_FILTER FilterHandle, FLT_FILTER_UNLOAD_FLAGS Flags) 
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(FilterHandle);
	UNREFERENCED_PARAMETER(Flags);

	KbCallbacks::StopObHandlesFilter();
	KbCallbacks::StopPsProcessFilter();
	KbCallbacks::StopPsThreadFilter();
	KbCallbacks::StopPsImageFilter();

	Communication::StopServer();
}

VOID OnDriverCreate(PDEVICE_OBJECT DeviceObject, PFLT_FILTER FilterHandle, PIRP Irp, PIO_STACK_LOCATION IrpStack)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(FilterHandle);
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(IrpStack);
}

VOID OnDriverCleanup(PDEVICE_OBJECT DeviceObject, PFLT_FILTER FilterHandle, PIRP Irp, PIO_STACK_LOCATION IrpStack)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(FilterHandle);
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(IrpStack);
}

VOID OnDriverClose(PDEVICE_OBJECT DeviceObject, PFLT_FILTER FilterHandle, PIRP Irp, PIO_STACK_LOCATION IrpStack)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(FilterHandle);
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(IrpStack);
}

