#include <ntddk.h> 

VOID OnDriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	DbgPrint("Driver unload routine triggered!\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	if (!pDriverObject)
		return STATUS_FAILED_DRIVER_ENTRY;

	pDriverObject->DriverUnload = OnDriverUnload;

	DbgPrint("Hello World!\n");

	return STATUS_SUCCESS;
}


