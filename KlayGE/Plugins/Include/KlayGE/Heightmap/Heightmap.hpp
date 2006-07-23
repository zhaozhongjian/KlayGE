// HeightMap.hpp
// KlayGE HeightMap地形生成类 头文件
// Ver 2.0.0
// 版权所有(C) 龚敏敏, 2003-2006
// Homepage: http://klayge.sourceforge.net
//
// 3.4.0
// 使用boost::function (2006.7.23)
//
// 2.0.0
// 初次建立 (2003.10.5)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#ifndef _HEIGHTMAP_HPP
#define _HEIGHTMAP_HPP

#include <vector>
#include <boost/function.hpp>

#ifdef KLAYGE_DEBUG
	#pragma comment(lib, "KlayGE_Terrain_HeightMap_d.lib")
#else
	#pragma comment(lib, "KlayGE_Terrain_HeightMap.lib")
#endif

namespace KlayGE
{
	// 高度图地形生成
	/////////////////////////////////////////////////////////////////////////////////
	class HeightMap
	{
	public:
		void BuildTerrain(float start_x, float start_y, float end_x, float end_y, float span_x, float span_y,
			std::vector<float3>& vertices, std::vector<uint16_t>& indices,
			boost::function<float(float, float)> HeightFunc);
	};
}

#endif		// _HEIGHTMAP_HPP
