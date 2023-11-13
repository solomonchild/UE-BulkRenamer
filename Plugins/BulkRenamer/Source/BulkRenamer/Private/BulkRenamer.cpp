#include "BulkRenamer.h"
#include "BulkRenamerStyle.h"
#include "BulkRenamerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "LevelEditor.h"

static const FName BulkRenamerTabName("BulkRenamer");

#define LOCTEXT_NAMESPACE "FBulkRenamerModule"


static void BuildMenu(class FMenuBuilder& menuBuilder, TArray<AActor*> SelectedActors)
{
			menuBuilder.AddEditableText(
				NSLOCTEXT("BulkRelabel", "MenuLabel", "New label:"), 
				NSLOCTEXT("BulkRelabel", "MenuHint", "A label for actor(s)"), 
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit"), 
				NSLOCTEXT("BulkRelabel", "DefaultText", ""),

				FOnTextCommitted::CreateLambda([=](const FText& name, ETextCommit::Type type) {
					if (type == ETextCommit::OnEnter) {
						unsigned actorIndex = 0;
						for (auto* actor : SelectedActors) {
							auto newName = FString::Printf(TEXT("%s%u"), *name.ToString(), actorIndex++);
							actor->SetActorLabel(*newName);
						}
					}
			}));
}


static TSharedRef<FExtender> OnExtendLevelEditorMenu(const TSharedRef<FUICommandList> CommandList, TArray<AActor*> SelectedActors)
{
	TSharedRef<FExtender> Extender(new FExtender());
	Extender->AddMenuExtension (
		"ActorOptions",
		EExtensionHook::After, 
		nullptr, 
		FMenuExtensionDelegate::CreateStatic(&BuildMenu, SelectedActors));
	return Extender;
}

void FBulkRenamerModule::OnPostEngineInit()
{
}

void FBulkRenamerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBulkRenamerStyle::Initialize();
	FBulkRenamerStyle::ReloadTextures();

	FBulkRenamerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBulkRenamerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FBulkRenamerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBulkRenamerModule::RegisterMenus));

	//FCoreDelegates::OnPostEngineInit.AddRaw(this, &FBulkRenamerModule::OnPostEngineInit);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	auto& allExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	auto LevelEditorMenuExtenderDelegate = FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateStatic(OnExtendLevelEditorMenu);
	allExtenders.Add(LevelEditorMenuExtenderDelegate);
}


void FBulkRenamerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FBulkRenamerStyle::Shutdown();

	FBulkRenamerCommands::Unregister();
}

void FBulkRenamerModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FBulkRenamerModule::PluginButtonClicked()")),
							FText::FromString(TEXT("BulkRenamer.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FBulkRenamerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBulkRenamerCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBulkRenamerCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBulkRenamerModule, BulkRenamer)