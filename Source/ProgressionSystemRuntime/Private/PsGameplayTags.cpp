// Copyright (c) Yevhenii Selivanov.

#include "PsGameplayTags.h"

namespace PsGameplayTags
{
	namespace UI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Widget_EndGame, "UI.Widget.ProgressionSystem.EndGame", "Widget tag for the Progression System end game widget");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Widget_MenuOverlay, "UI.Widget.ProgressionSystem.MenuOverlay", "Widget tag for the Progression System menu overlay widget");
	} // namespace UI

	namespace Menu
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Spot, "NMM.Spot", "Skeletal mesh actor should own this tag, used to prevent initializing PS spots on other skeletal mesh actors, like from cinematics");
	} // namespace Menu

	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameProgressionCompleted, "Event.ProgressionSystem.GameProgressionCompleted", "Event that fires when a player completes all levels in the progression system");
	} // namespace Event
} // namespace PsGameplayTags
