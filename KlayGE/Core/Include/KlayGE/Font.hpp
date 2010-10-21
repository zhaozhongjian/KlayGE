// Font.hpp
// KlayGE Font类 头文件
// Ver 3.10.0
// 版权所有(C) 龚敏敏, 2003-2010
// Homepage: http://www.klayge.org
//
// 3.10.0
// RenderText速度增加50% (2010.3.9)
//
// 3.9.0
// 增加了KFontLoader (2009.10.16)
//
// 3.7.0
// 新的基于distance的字体格式 (2008.2.13)
//
// 3.6.0
// 增加了Rect对齐的方式 (2007.6.5)
//
// 3.3.0
// 支持渲染到3D位置 (2006.5.20)
//
// 2.8.0
// 增加了pool (2005.8.10)
//
// 2.3.0
// 使用FreeType实现字体读取 (2004.12.26)
//
// 2.0.0
// 初次建立 (2003.8.18)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#ifndef _FONT_HPP
#define _FONT_HPP

#pragma once

#ifndef KLAYGE_CORE_SOURCE
#define KLAYGE_LIB_NAME KlayGE_Core
#include <KlayGE/config/auto_link.hpp>
#endif

#include <KlayGE/PreDeclare.hpp>

#include <KlayGE/Rect.hpp>
#include <KlayGE/Renderable.hpp>
#include <KlayGE/RenderableHelper.hpp>

#include <list>
#include <map>
#include <vector>
#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 6011 6334)
#endif
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(pop)
#endif

#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
#include <boost/pool/pool_alloc.hpp>
#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(pop)
#endif

namespace KlayGE
{
	class KLAYGE_CORE_API KFontLoader
	{
	public:
#ifdef KLAYGE_PLATFORM_WINDOWS
	#pragma pack(push, 1)
#endif
		struct font_info
		{
			int16_t top;
			int16_t left;
			uint16_t width;
			uint16_t height;
		};
#ifdef KLAYGE_PLATFORM_WINDOWS
	#pragma pack(pop)
#endif

	public:
		explicit KFontLoader(std::string const & font_name);

		uint32_t CharSize() const;
		int16_t DistBase() const;
		int16_t DistScale() const;

		std::pair<int32_t, Vector_T<uint16_t, 2> > CharIndexAdvance(wchar_t ch) const;
		int32_t CharIndex(wchar_t ch) const;
		Vector_T<uint16_t, 2> CharAdvance(wchar_t ch) const;

		font_info const & CharInfo(int32_t offset) const;
		void DistanceData(uint8_t* p, uint32_t pitch, int32_t offset) const;

	private:
		uint32_t char_size_;
		int16_t dist_base_;
		int16_t dist_scale_;
		boost::unordered_map<int32_t, std::pair<int32_t, Vector_T<uint16_t, 2> >, boost::hash<int32_t>, std::equal_to<int32_t>,
			boost::fast_pool_allocator<std::pair<int32_t, std::pair<int32_t, Vector_T<uint16_t, 2> > > > > char_index_advance_;
		std::vector<font_info> char_info_;
		std::vector<size_t> distances_addr_;
		std::vector<uint8_t> distances_lzma_;
	};

	class KLAYGE_CORE_API FontRenderable : public RenderableHelper
	{
	public:
		explicit FontRenderable(std::string const & font_name);

		RenderTechniquePtr const & GetRenderTechnique() const;

		void UpdateBuffers();
		void OnRenderBegin();
		void OnRenderEnd();

		void Render();

		Size_T<uint32_t> CalcSize(std::wstring const & text, uint32_t font_height);

		void AddText2D(float sx, float sy, float sz,
			float xScale, float yScale, Color const & clr, std::wstring const & text, uint32_t font_height);
		void AddText2D(Rect const & rc, float sz,
			float xScale, float yScale, Color const & clr, std::wstring const & text, uint32_t font_height, uint32_t align);
		void AddText3D(float4x4 const & mvp, Color const & clr, std::wstring const & text, uint32_t font_height);

	private:
		void AddText(Rect const & rc, float sz,
			float xScale, float yScale, Color const & clr, std::wstring const & text, uint32_t font_height, uint32_t align);

		void AddText(float sx, float sy, float sz,
			float xScale, float yScale, Color const & clr, std::wstring const & text, uint32_t font_height);

		void UpdateTexture(std::wstring const & text);

	private:
		struct CharInfo
		{
			Rect_T<float> rc;
			uint64_t tick;
		};

#ifdef KLAYGE_PLATFORM_WINDOWS
	#pragma pack(push, 1)
#endif
		struct FontVert
		{
			float3 pos;
			uint32_t clr;
			float2 tex;

			FontVert()
			{
			}
			FontVert(float3 const & pos, uint32_t clr, float2 const & tex)
				: pos(pos), clr(clr), tex(tex)
			{
			}
		};
#ifdef KLAYGE_PLATFORM_WINDOWS
	#pragma pack(pop)
#endif

		bool restart_;
		bool dirty_;

		boost::unordered_map<wchar_t, CharInfo, boost::hash<wchar_t>, std::equal_to<wchar_t>,
			boost::fast_pool_allocator<std::pair<wchar_t, CharInfo> > > charInfoMap_;

		uint32_t curX_, curY_;

		bool three_dim_;

		std::vector<FontVert>	vertices_;
		std::vector<uint16_t>	indices_;

		GraphicsBufferPtr vb_;
		GraphicsBufferPtr ib_;

		TexturePtr		dist_texture_;
		TexturePtr		a_char_texture_;
		RenderEffectPtr	effect_;

		RenderEffectParameterPtr half_width_height_ep_;
		RenderEffectParameterPtr mvp_ep_;

		KFontLoader kfont_loader_;

		uint64_t tick_;
	};

	// 在3D环境中画出文字
	/////////////////////////////////////////////////////////////////////////////////
	class KLAYGE_CORE_API Font
	{
	public:
		// 字体建立标志
		enum FontStyle
		{
			FS_TwoSided		= 1UL << 0,
			FS_Cullable		= 1UL << 1
		};

		enum FontAlign
		{
			FA_Hor_Left		= 1UL << 0,
			FA_Hor_Center	= 1UL << 1,
			FA_Hor_Right	= 1UL << 2,

			FA_Ver_Top		= 1UL << 3,
			FA_Ver_Middle	= 1UL << 4,
			FA_Ver_Bottom	= 1UL << 5
		};

	public:
		Font(RenderablePtr const & font_renderable, uint32_t flags = 0);

		Size_T<uint32_t> CalcSize(std::wstring const & text, uint32_t font_size);
		void RenderText(float x, float y, Color const & clr,
			std::wstring const & text, uint32_t font_size);
		void RenderText(float x, float y, float z, float xScale, float yScale, Color const & clr,
			std::wstring const & text, uint32_t font_size);
		void RenderText(Rect const & rc, float z, float xScale, float yScale, Color const & clr,
			std::wstring const & text, uint32_t font_size, uint32_t align);
		void RenderText(float4x4 const & mvp, Color const & clr, std::wstring const & text, uint32_t font_size);

	private:
		RenderablePtr	font_renderable_;
		uint32_t		fso_attrib_;
	};
}

#endif		// _FONT_HPP
