#pragma once

#define ONE_NAME "OneGameHostingSDK"
#define ONE_VERSION "0.9.0"
// clang-format off
#define ONE_VERSION_MAJOR 0
#define ONE_VERSION_MINOR 9
#define ONE_VERSION_PATCH 0
// clang-format on

namespace i3d {
namespace one {

// References
// Original link: https://nfrechette.github.io/2020/07/18/zero_overhead_backward_compat/
//
// Zero overhead backward compatibility
// 18 Jul 2020
// The Animation Compression Library finally supports backward compatibility (going
// forward once 2.0 comes out). I’m really happy with how it turned out so I thought I
// would write a bit about how the ACL decompression is designed.
//
// The API
// At runtime, decompressing animation data could not be easier:
//
// acl::decompression_context<custom_decompression_settings> context;
// context.initialize(compressed_data);
//
// // Seek 1.0 second into our compressed animation
// // and don't use rounding to interpolate
// context.seek(1.0f, acl::sample_rounding_policy::none);
//
// custom_writer writer(output_data);
// context.decompress_tracks(writer);
// A small context object is created and bound to our compressed data. Its construction is
// cheap enough that it can be done on the stack on demand. It can then subsequently be
// used (and re-used) to seek and decompress.
//
// A key design goal is to have as little overhead as possible: pay only for what you use.
// This is achieved through templating in two ways:
//
// The custom_decompression_settings argument on the context object controls what features
// are enabled or disabled. The custom_writer wraps whatever container you might be using
// in your game engine to represent the animation data. This is to make sure no extra
// copying is required. The decompression settings are where the magic happens.
//
// Compile time user control
// There are many game engines out there each handling animation in their own specific
// way. In order to be able to integrate as seamlessly as possible, ACL exposes a small
// struct that can be overriden to control its behavior. By leveraging constexpr and
// templating, features that aren’t used or needed can be removed entirely at compile time
// to ensure zero cost at runtime.
//
// Here is how it looks:
//
// struct decompression_settings
// {
//   // Whether or not to clamp the sample time when `seek(..)`
//   // is called. Defaults to true.
//   static constexpr bool clamp_sample_time() { return true; }
//
//   // Whether or not the specified track type is supported.
//   // Defaults to true.
//   // If a track type is statically known not to be supported,
//   // the compiler can strip the associated code.
//   static constexpr bool is_track_type_supported(track_type8 /*type*/)
//   { return true; }
//
//   // Other stuff ...
//
//   // Which version we should optimize for.
//   // If 'any' is specified, the decompression context will
//   // support every single version with full backwards
//   // compatibility.
//   // Using a specific version allows the compiler to
//   // statically strip code for all other versions.
//   // This allows the creation of context objects specialized
//   // for specific versions which yields optimal performance.
//   static constexpr compressed_tracks_version16 version_supported()
//   { return compressed_tracks_version16::any; }
//
//   // Whether the specified rotation/translation/scale format
//   // are supported or not.
//   // Use this to strip code related to formats you do not need.
//   static constexpr bool is_rotation_format_supported(rotation_format8 /*format*/)
//   { return true; }
//
//   // Other stuff ...
//
//   // Whether rotations should be normalized before being
//   // output or not. Some animation runtimes will normalize
//   // in a separate step and do not need the explicit
//   // normalization.
//   // Enabled by default for safety.
//   static constexpr bool normalize_rotations() { return true; }
// };
// Extending this is simple and clean:
//
// struct default_transform_decompression_settings : public decompression_settings
// {
//   // Only support transform tracks
//   static constexpr bool is_track_type_supported(track_type8 type)
//   { return type == track_type8::qvvf; }
//
//   // By default, we only support the variable bit rates as
//   // they are generally optimal
//   static constexpr bool is_rotation_format_supported(rotation_format8 format)
//   { return format == rotation_format8::quatf_drop_w_variable; }
//
//   // Other stuff ...
// };
// A new struct is created to inherit from the desired decompression settings and specific
// functions are defined to hide the base implementations, thus replacing them.
//
// By templating the decompression_context with the settings structure, it can be used to
// determine everything needed at compile time:
//
// Which memory representation is needed depending on whether we are decompressing scalar
// or transform tracks. Which algorithm version to support and optimize for. Which
// features to strip when they aren’t needed. This is much nicer than the common C way to
// use #define macros. By using a template argument, multiple setting objects can easily
// be created (with type safety) and used within the same application or file.
//
// Backward compatibility
// By using the decompression_settings, we can specify which version we optimize for. If
// no specific version is provided (the default behavior), we will branch and handle all
// supported versions. However, if a specific version is provided, we can strip the code
// for all other versions removing any runtime overhead. This is clean and simple thanks
// to templates.
//
// template<compressed_tracks_version16 version>
// struct decompression_version_selector {};
//
// // Specialize for ACL 2.0's format
// template<> struct
// decompression_version_selector<compressed_tracks_version16::v02_00_00>
// {
//   static bool is_version_supported(compressed_tracks_version16 version)
//   { return version == compressed_tracks_version16::v02_00_00; }
//
//   template<class decompression_settings_type, class context_type>
//   ACL_FORCE_INLINE static bool initialize(context_type& context, const
//   compressed_tracks& tracks)
//   {
//     return acl_impl::initialize_v0<decompression_settings_type>(context, tracks);
//   }
//
//   // Other stuff ...
// };
//
// // Specialize to support all versions
// template<> struct
// decompression_version_selector<compressed_tracks_version16::any>
// {
//   static bool is_version_supported(compressed_tracks_version16 version)
//   {
//     return version >= compressed_tracks_version16::first && version <=
//     compressed_tracks_version16::latest;
//   }
//
//   template<class decompression_settings_type, class context_type>
//   static bool initialize(context_type& context, const compressed_tracks& tracks)
//   {
//     // TODO: Note that the `any` decompression can be optimized further to avoid a
//     complex switch on every call. const compressed_tracks_version16 version =
//     tracks.get_version(); switch (version)
//     {
//     case compressed_tracks_version16::v02_00_00:
//       return
//       decompression_version_selector<compressed_tracks_version16::v02_00_00>::initialize<decompression_settings_type>(context,
//       tracks);
//     default:
//       ACL_ASSERT(false, "Unsupported version");
//       return false;
//     }
//   }
//
//   // Other stuff ...
// };
// This is ideal for many game engines. For example Unreal Engine 4 always compresses
// locally and caches the result in its Derived Data Cache. This means that the compressed
// format is always the latest one used by the plugin. As such, UE4 only needs to support
// a single version and it can do so without any overhead.
//
// Other game engines might choose to support the latest two versions, emiting a warning
// to recompress old animations while still being able to support them with very little
// overhead: a single branch to pick which context to use.
//
// More general applications might opt to support every version (e.g. a glTF viewer).
//
// Note that backward compatibility will only be supported for official releases as the
// develop branch is constantly subject to change.
//
// Conclusion
// This C++ customization pattern is clean and simple to use and it allows a compact API
// with a rich feature set. It was present in a slightly different form ever since ACL 0.1
// and the more I use it, the more I love it.
//
// In fact, in my opinion the Animation Compression Library and Realtime Math contain some
// of the best code (quality wise) that I’ve ever written in my career. Free from time or
// budget constraints, I can carefully craft each facet to the best of my ability.
//
// ACL 2.0 continues to progress nicely. It is still missing a few features but it is
// already an amazing step up from 1.3.

enum class ArcusVersion { V2 };

struct arcus_protocol {
    static constexpr ArcusVersion current_version() {
        // clang-format off
        return ArcusVersion::V2;
        // clang-format on
    }
};

template <ArcusVersion version>
struct arcus_selector {};

template <>
struct arcus_selector<ArcusVersion::V2> {
    static constexpr bool is_version_supported(ArcusVersion version) {
        return version == ArcusVersion::V2;
    }
};

}  // namespace one
}  // namespace i3d
