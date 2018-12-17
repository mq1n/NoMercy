#pragma once

class ObCallbacks final
{
	enum ObCallbackType
	{
		ctAll,
		ctProcesses,
		ctThreads,
		ctMaxValue // Same as ctAll
	};

	using _ObRegisterCallbacks = NTSTATUS(*)(POB_CALLBACK_REGISTRATION CallbackRegistration, PVOID* RegistrationHandle);
	using _ObUnRegisterCallbacks = NTSTATUS(*)(PVOID RegistrationHandle);

	public:
		ObCallbacks(const ObCallbacks&) = delete;
		ObCallbacks(ObCallbacks&&) = delete;
		ObCallbacks& operator = (const ObCallbacks&) = delete;
		ObCallbacks& operator = (ObCallbacks&&) = delete;

		ObCallbacks();
		ObCallbacks(OPTIONAL POB_PRE_OPERATION_CALLBACK PreCallback, OPTIONAL POB_POST_OPERATION_CALLBACK PostCallback, OPTIONAL PVOID RegistrationContext = NULL, 
			ObCallbackType ObjectType = ObCallbackType::ctAll, OB_OPERATION OperationType = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE);
		~ObCallbacks();

		NTSTATUS SetupCallbacks(OPTIONAL POB_PRE_OPERATION_CALLBACK PreCallback, OPTIONAL POB_POST_OPERATION_CALLBACK PostCallback = NULL, 
			OPTIONAL PVOID RegistrationContext = NULL, ObCallbackType ObjectType = ObCallbackType::ctAll, 
			OB_OPERATION OperationType = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE);
		VOID RemoveCallbacks();

	private:
		PVOID RegistrationHandle;
		_ObRegisterCallbacks DynObRegisterCallbacks;
		_ObUnRegisterCallbacks DynObUnRegisterCallbacks;
};
