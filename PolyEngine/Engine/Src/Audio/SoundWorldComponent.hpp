#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Audio/SoundSystem.hpp>
#include <Audio/SoundListenerComponent.hpp>

typedef struct ALCcontext_struct ALCcontext;

namespace Poly
{
	class Scene;

	/// Component containing OpenAL context used in sound rendering.
	/// Only one context can be active in time.
	/// There can be only one context per world.
	/// @see SoundSystem
	class ENGINE_DLLEXPORT SoundWorldComponent : public ComponentBase
	{
	friend void SoundSystem::SetWorldCurrent(Scene* world);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SoundWorldComponent) { NO_RTTI_PROPERTY(); }
		/// Creates context from current device (stored in engine).
		/// @param world - world which will  be associated with just created context.
		/// @see OpenALDevice
		SoundWorldComponent(Scene* world);

		/// Releases stored context.
		~SoundWorldComponent();

		const Dynarray<SoundListenerComponent>& GetListeners() const { return Listeners; };

	private:
		ALCcontext* Context;
		Dynarray<SoundListenerComponent> Listeners;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SoundWorldComponent)

} // namespace Poly