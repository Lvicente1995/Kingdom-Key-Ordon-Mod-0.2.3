# Kingdom Key — Ordon Sword

Custom **v0.2.3** source for **Dusklight 2.0.3**, based on the user's v0.2.2a universal source. Adds Kingdom Key artwork to the Collection/pause menu, standard button HUD and touch button UI, plus the English item name **Kingdom Key**. The portable audio implementation and eight-platform build workflow are retained.

## Install

Build the project with the included GitHub Actions workflow and download the `mod-combined` artifact. Inside is the universal `kingdom_key.dusk`; install that single file through Dusklight's mod manager/data-folder `mods` directory. Do not extract the `.dusk`. Remove older Kingdom Key versions so only one package with this mod ID is installed.

The universal bundle is intended to contain native libraries for every target in Dusklight's official mod-template matrix while sharing one copy of `mod.json` and `res/`.

## Appearance and movement

- The button icon uses the user-approved Kingdom Key artwork; the Collection icon mirrors that same artwork horizontally, matching the requested menu orientation. Shading and outlines follow the original Twilight Princess sword icon.
- The English equipment title and sword-acquisition message say **Kingdom Key**. Existing formatting, controller glyphs and other languages are preserved.

- Original full-detail Kingdom Key geometry: 52,072 triangles, silver shaft and crown teeth, gold guard, blue neck, black grip, and Mickey charm.
- Grip aligned to the Ordon Sword. Tip reaches 99.37 game units versus the original 99.67, preserving the original attack reach.
- Fourteen individual chain links plus a heavier terminal charm move under gravity and inertia. Motion reacts to the weapon rather than repeating a baked animation.
- Chain constraints retain their length during fast motion. Basic torso and floor contacts reduce clipping. Motion resets safely when changing form, drawing/stowing, or teleporting.
- The Keyblade materializes in Link's hand and vanishes when put away. Its body, chain and charm leave together; no weapon, scabbard or weapon shadow remains on Link's back.
- Summoning, dismissal and enemy-hit effects use selected Kingdom Hearts III textures, effect meshes and Cascade particle settings from the user's installed copy, adapted to Dusklight's renderer. These replace the earlier procedural approximation.
- Authentic Kingdom Key audio accompanies appearing, disappearing and confirmed enemy contacts. KHIII references the same appearance effect and sound cue `se02001_010` for both appearing and disappearing. Seven original hit-family cues provide impact variations.
- Link's sword arm no longer performs the normal reach-behind draw, sheath or victory-flourish gesture. An independent animation preserves the shield arm's native movement and transfers the shield at its hand/back contact frame. It plays at 75% native speed with a five-tick settling blend, following feedback that the initial version moved too quickly. Scripted event animations retain their native timing.
- Metal highlights use source metalness and roughness with the game's sunlight, room lights and fog. Brightness is balanced for the supplied solid materials.
- Native frame interpolation, inventory-model rendering, custom shadow geometry, and mirror rendering are implemented.

## Current scope

Version 0.2.3 is a UI/name update to the supplied universal source. Gameplay, physics, sound and particle behavior are preserved. The user reported a successful full matrix build and Android runtime for the supplied v0.2.2a baseline; that evidence does not automatically validate this new revision. See `VALIDATION.md` for revision-specific checks.

Sword damage, hitboxes, attack animations and attack trails retain Ordon behavior. Native equipment changes complete immediately while the cosmetic weapon transition and shield gesture run independently. The Keyblade hit effect/audio trigger is limited to confirmed, nonblocked enemy contacts and deduplicated per target per game tick. It replaces the native generic hitmark only for those accepted contacts; collision, damage and enemy reactions remain unchanged. Original draw/sheath sounds are suppressed when the replacement cue is available. Other game audio and separate pickup/cutscene prop models retain their originals. Only the Ordon Sword UI artwork and the two English messages that name it are replaced; the original item description remains contextual.

The inventory preview uses a static chain pose. Physics uses approximate torso/floor contact rather than full environment or chain self-collision. Compatibility with other sword replacers, remote co-op avatars and every gameplay situation has not been established.

The Blender preview is a studio render, not an in-game screenshot. The game's metal lighting approximates the source material. KHIII particle timing, masks, mesh geometry and vertex color/alpha gradients are retained where supported. The source effect basis (+X along the blade, +Z toward the teeth) is aligned to the replacement with a 1.26 scale. Engine-specific material effects such as Fresnel, erosion, lighting and compositing remain approximations; this is not a claim of pixel-identical KHIII rendering.

## Asset sources and storage

The weapon model comes from the supplied `kingdom-key.zip`. The new UI icon was generated from the user's approved Kingdom Key reference and the original game icon style; its source and exact prompt are documented in `ARTWORK.md`. Selected sounds and visual-effect assets come from the user's local Kingdom Hearts III installation. Its archives were read in place; only the needed files were extracted into the D: workspace. No game or full archive was copied, and no extracted game assets were placed on C:.

The original Twilight Princess ISO, original Dusklight installation and original saves were not modified during development. Testing uses an isolated Dusklight copy and a copied save. Installing this mod adds a package to Dusklight's mod directory; it does not patch either game's files.

## Editable source

`../model/kingdom-key-rigged.blend` contains the original detailed mesh with separate bones for all fourteen links and the charm. `src/mod.cpp` contains the Dusklight integration; `src/chain_physics.hpp` contains the solver. `src/keyblade_presence.hpp` controls visibility, `src/shield_gesture.hpp` controls the independent shield gesture, and `src/keyblade_audio.hpp` supplies native sound playback. `src/kh3_fx.hpp`, `src/kh3_fx_data.hpp` and `res/kh3fx/` contain the adapted particle renderer, settings and assets; texture/material metadata records the source masks, colors, tiling and addressing. `src/keyblade_ui.hpp` registers the shared icon texture and supplies the touch button source; Collection drawing mirrors only its own icon pane. `src/keyblade_text.hpp` uses MessageService for processed names and the pickup message, plus a bounded hook for the equipment menu's direct string helper. The older `src/summon_fx.hpp` and its tests document the earlier procedural iteration.

`tools/kingdom-key-mesh.json` and `tools/export_mesh.py` regenerate `res/kingdom_key.mesh` with Python 3. `tools/convert_kh3_fx_meshes.py` and its validation report document conversion of the selected KHIII effect geometry.

## Cross-platform support

This source tree follows Dusklight's official mod-template matrix: Windows AMD64/ARM64, Linux x86_64/aarch64, macOS arm64/x86_64, iOS arm64, and Android aarch64. See `CROSS_PLATFORM.md` for details. GitHub Actions builds the platform libraries and merges them into one multi-platform `.dusk`.

## Build

The recommended distribution build is `.github/workflows/build.yml`. It follows Dusklight's current official mod-template matrix and creates per-platform artifacts for:

- Windows AMD64 and ARM64
- Linux x86_64 and aarch64
- macOS Apple Silicon and Intel
- iOS arm64
- Android aarch64

When every matrix job succeeds, `Combine bundles` creates the `mod-combined` artifact containing one universal `kingdom_key.dusk`. The workflow also supports manual runs through `workflow_dispatch`.

The project pins the Dusklight 2.0.3 SDK commit `40457c6adb381928e4b5fef6ed459ed291edd5e2`. Dusklight 2.0.2 is not supported by this source. A local build can be made with:

```sh
cmake -B build
cmake --build build --parallel
```

A local build only produces a package for the current host/target; use the Actions matrix for distribution. `.gitattributes` and the LF-normalized configured `mod.json` prevent cross-platform metadata byte mismatches during bundle merging.

The `tools/` folder contains the existing physics, geometry, interpolation, presence, shield-animation, and KHIII effect regression checks. The v0.2.3 UI and message checks are in `tools/ui_regression.cpp` and `tools/keyblade_text_test.cpp`; successful CI compilation does not by itself prove runtime audio behavior on every device, so test the combined bundle on representative hardware.

Official references: [Dusklight mod template](https://github.com/TwilitRealm/mod-template), [v2.0.3 modding API](https://github.com/TwilitRealm/dusklight/blob/v2.0.3/docs/modding.md).
