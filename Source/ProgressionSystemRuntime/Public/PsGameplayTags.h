// Copyright (c) Yevhenii Selivanov.

#pragma once

// UE
#include "NativeGameplayTags.h" // UE_DECLARE_GAMEPLAY_TAG_EXTERN

namespace PsGameplayTags
{
	namespace UI
	{
		/** Widget tag for the Progression System end game widget */
		PROGRESSIONSYSTEMRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_EndGame);

		/** Widget tag for the Progression System menu overlay widget */
		PROGRESSIONSYSTEMRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_MenuOverlay);
	} // namespace UI

	namespace Menu
	{
		/** Skeletal mesh actor should own this tag, used to prevent initializing PS spots on other skeletal mesh actors, like from cinematics */
		PROGRESSIONSYSTEMRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Spot);
	} // namespace Menu

	namespace Event
	{
		/** Event that fires when a player completes all levels in the progression system */
		PROGRESSIONSYSTEMRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameProgressionCompleted);
	} // namespace Event
} // namespace PsGameplayTags
