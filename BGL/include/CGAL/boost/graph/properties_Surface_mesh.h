// Copyright (c) 2012 GeometryFactory (France). All rights reserved.
// All rights reserved.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Philipp Möller


#ifndef CGAL_PROPERTIES_SURFACE_MESH_H
#define CGAL_PROPERTIES_SURFACE_MESH_H

#include <CGAL/assertions.h>
#include <CGAL/Surface_mesh/Surface_mesh.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/squared_distance_3.h>

#include <CGAL/boost/graph/properties.h>

namespace CGAL {

template <typename Point>
class SM_edge_weight_pmap 
  : public boost::put_get_helper<typename CGAL::Kernel_traits<Point>::type::FT, SM_edge_weight_pmap<Point> >
{
  typedef CGAL::Surface_mesh<Point> SM;
public:
  typedef boost::readable_property_map_tag                category;
  typedef typename CGAL::Kernel_traits<Point>::type::FT   value_type;
  typedef value_type                                      reference;
  typedef typename SM::Edge_index                        key_type;

  SM_edge_weight_pmap(const CGAL::Surface_mesh<Point>& sm)
    : pm_(sm. template property_map< 
            typename SM::Vertex_index,
            typename SM::Point >("v:point").first),
      sm_(sm)
    {}

  value_type operator[](const key_type& e) const
  {
    return CGAL::sqrt(CGAL::squared_distance(pm_[source(e, sm_)],
                                             pm_[target(e, sm_)]));
  }

private:
   typename SM::template Property_map< typename SM::Vertex_index, 
                                       typename SM::Point > pm_;
  const SM& sm_;
};


template <typename K, typename VEF>
class SM_index_pmap : public boost::put_get_helper<int, SM_index_pmap<K,VEF> >
{
public:
  typedef boost::readable_property_map_tag category;
  typedef unsigned int                     value_type;
  typedef unsigned int                     reference;
  typedef VEF                              key_type;

  value_type operator[](const key_type& vd) const
  {
    return (size_t)vd;
  }
};

} // CGAL

// overloads and specializations in the boost namespace
namespace boost {
#if 1
template <typename Point, typename T>
struct property_map<CGAL::Surface_mesh<Point>, boost::vertex_property_t<T> >
{
  typedef CGAL::Surface_mesh<Point> SM;
  typedef typename SM:: template Property_map<typename SM::vertex_index,T> type;
  typedef type const_type;
};


#endif


//
// edge_weight
//

template <typename Point>
struct property_map<CGAL::Surface_mesh<Point>, boost::edge_weight_t >
{
  typedef CGAL::SM_edge_weight_pmap<Point> type;
  typedef CGAL::SM_edge_weight_pmap<Point> const_type;
};

template <typename Point>
typename property_map<CGAL::Surface_mesh<Point>, boost::edge_weight_t>::const_type
get(boost::edge_weight_t, const CGAL::Surface_mesh<Point>& sm)
{
  return CGAL::SM_edge_weight_pmap<Point>(sm);
}

template <typename Point>
typename CGAL::Kernel_traits<Point>::type::FT
get(boost::edge_weight_t, const CGAL::Surface_mesh<Point>& sm, 
    const typename boost::graph_traits<CGAL::Surface_mesh<Point> >::edge_descriptor& e)
{
  return CGAL::SM_edge_weight_pmap<Point>(sm)[e];
}

//
// vertex_index
//

template <typename K>
struct property_map<CGAL::Surface_mesh<K>, boost::vertex_index_t >
{
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::vertex_descriptor> type;
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::vertex_descriptor> const_type;
};

template <typename K>
CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::vertex_descriptor>
get(const boost::vertex_index_t&, const CGAL::Surface_mesh<K>&)
{
  return CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::vertex_descriptor>();
}

//
// face_index
//

template <typename K>
struct property_map<CGAL::Surface_mesh<K>, boost::face_index_t >
{
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::face_descriptor> type;
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::face_descriptor> const_type;
};

template <typename K>
typename property_map<CGAL::Surface_mesh<K>, boost::face_index_t>::const_type
get(const boost::face_index_t&, const CGAL::Surface_mesh<K>&)
{
  return CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::face_descriptor>();
}

//
// edge_index
//

template <typename K>
struct property_map<CGAL::Surface_mesh<K>, boost::edge_index_t >
{
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::edge_descriptor> type;
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::edge_descriptor> const_type;
};

template <typename K>
CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::edge_descriptor>
get(const boost::edge_index_t&, const CGAL::Surface_mesh<K>&)
{
  return CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::edge_descriptor>();
}

//
// halfedge_index
//

template <typename K>
struct property_map<CGAL::Surface_mesh<K>, boost::halfedge_index_t >
{
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::halfedge_descriptor> type;
  typedef CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::halfedge_descriptor> const_type;
};

template <typename K>
CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::halfedge_descriptor>
get(const boost::halfedge_index_t&, const CGAL::Surface_mesh<K>&)
{
  return CGAL::SM_index_pmap<K, typename boost::graph_traits<CGAL::Surface_mesh<K> >::halfedge_descriptor>();
}

//
// vertex_point
// 

template<typename P>
struct property_map<CGAL::Surface_mesh<P>, CGAL::vertex_point_t >
{
  typedef CGAL::Surface_mesh<P> SM; 

  typedef typename
    SM::template Property_map< typename SM::Vertex_index, 
                               P
                               > type;
  
  typedef type const_type;
  
};

template<typename K>
typename property_map<CGAL::Surface_mesh<K>, CGAL::vertex_point_t >::const_type
get(CGAL::vertex_point_t, const CGAL::Surface_mesh<K>& g) {
  return g.points();
}


// get for intrinsic properties
#define CGAL_SM_INTRINSIC_PROPERTY(RET, PROP, TYPE)                     \
  template<typename Point>                                              \
  RET                                                                   \
  get(PROP p, const CGAL::Surface_mesh<Point>& sm,                      \
      typename boost::graph_traits< CGAL::Surface_mesh<Point> >::TYPE x) \
  { return get(get(p, sm), x); }                                        \

CGAL_SM_INTRINSIC_PROPERTY(std::size_t, boost::vertex_index_t, vertex_descriptor)
CGAL_SM_INTRINSIC_PROPERTY(std::size_t, boost::edge_index_t, edge_descriptor)
CGAL_SM_INTRINSIC_PROPERTY(std::size_t, boost::halfedge_index_t, halfedge_descriptor)
CGAL_SM_INTRINSIC_PROPERTY(std::size_t, boost::face_index_t, face_descriptor)
CGAL_SM_INTRINSIC_PROPERTY(Point&, CGAL::vertex_point_t, vertex_descriptor)

#undef CGAL_SM_INTRINSIC_PROPERTY

// put for intrinsic properties
// only available for vertex_point
template<typename K>
void
put(CGAL::vertex_point_t p, const CGAL::Surface_mesh<K>& g,
    typename boost::graph_traits< CGAL::Surface_mesh<K> >::vertex_descriptor x,
    const K& point) {
  typedef CGAL::Surface_mesh<K> SM;
  CGAL_assertion(g.is_valid(x));
  typename SM::template Property_map< typename boost::graph_traits<SM>::vertex_descriptor, 
                    K> prop = get(p, g);
  prop[x] = point;
}

} // boost

#if 0
// 
namespace CGAL {
template <typename Point, typename T>
typename boost::property_map<CGAL::Surface_mesh<Point>, boost::vertex_property_t<T> >::const_type
get(boost::vertex_property_t<T> vprop, const CGAL::Surface_mesh<Point>& sm)
{
  return sm.template get_property_map<typename CGAL::Surface_mesh<Point>::Vertex_index, T>(vprop.s).first;
}

template <typename Point, typename T>
typename boost::property_map<CGAL::Surface_mesh<Point>, boost::vertex_property_t<T> >::const_type
add(boost::vertex_property_t<T> vprop, CGAL::Surface_mesh<Point>& sm)
{
  return sm.template add_property_map<typename CGAL::Surface_mesh<Point>::Vertex_index, T>(vprop.s, vprop.t).first;
}

template <typename Pmap,typename P>
void
remove(Pmap pm, CGAL::Surface_mesh<P>& sm)
{
  return sm.remove_property_map(pm);
}
} // namespace CGAL
#endif

#endif /* CGAL_PROPERTIES_SURFACE_MESH_H */
