#include "HexMapGameMode.h"

#include "HexMapPlayerController.h"

AHexMapGameMode::AHexMapGameMode()
{
	PlayerControllerClass = AHexMapPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
}
