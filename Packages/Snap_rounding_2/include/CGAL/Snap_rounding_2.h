// ======================================================================
//
// Copyright (c) The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------
//
// release       : $CGAL_Revision: CGAL-2.3-I-24 $
// release_date  : $CGAL_Date: 2000/12/29 $
//
// file          : include/CGAL/Snap_rounding_2.h
// package       : arr (1.73)
// maintainer    : Eli Packer <elip@post.tau.ac.il>
// author(s)     : Eli Packer
// coordinator   : Tel-Aviv University (Dan Halperin <danha@post.tau.ac.il>)
//
// ======================================================================
#ifndef CGAL_SR_2_H
#define CGAL_SR_2_H

#include <CGAL/leda_rational.h> 

#include <iostream>

//#ifndef CGAL_ENUM_H
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
// @@@@ special includes for pm
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Pm_default_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Planar_map_2.h>
#include <CGAL/Pm_with_intersections.h>
// @@@@ end of special includes
#include <CGAL/enum.h>
#include <CGAL/predicates_on_points_2.h>
#include <CGAL/Random.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/intersection_2.h>

//#endif

//#include <CGAL/Sweep_line_tight_2.h>
#include <CGAL/Sweep_line_2.h>

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_polyline_traits.h>
#include <list>
#include <set>
#include <CGAL/leda_real.h>
#include "Snap_rounding_kd_2.h"

#include <CGAL/utility.h>
#include <CGAL/Iterator_project.h>
#include <CGAL/function_objects.h>

CGAL_BEGIN_NAMESPACE

template<class Rep_>
class Segment_data {

typedef Rep_                                Rep;
typedef typename Rep::FT                    NT;
typedef typename Rep::Segment_2             Segment_2;
typedef typename Rep::Point_2               Point_2;

private:
 Point_2 p;
 Point_2 q;

 Rep_   _gt;

public:
  Segment_data();
  Segment_data(Point_2 p_inp,Point_2 q_inp);
  Segment_2 segment() const {return(Segment_2(p,q));}
  Point_2 source() const {return(p);}
  Point_2 target() const {return(q);}
  inline void set_data(Point_2 inp_p,Point_2 inp_q);
  void determine_direction();
  bool equal(Segment_2 s);
  Segment_data(const Segment_data &other);
};

template<class Rep_>
class Hot_Pixel {

typedef Rep_                       Rep;
typedef typename Rep::FT           NT;
typedef typename Rep::Segment_2    Segment_2;
typedef typename Rep::Point_2      Point_2;

private:
  // p is the center of the hot pixel
  Point_2 p;
  Point_2 p_left;
  Point_2 p_right;
  Point_2 p_down;
  Point_2 p_up;
  Rep_ _gt;
  NT pixel_size;
  Segment_2 *right_seg;
  Segment_2 *left_seg;
  Segment_2 *top_seg;
  Segment_2 *bot_seg;

public:
  template<class Out>
  void draw(Out &o) const;
  Hot_Pixel(Point_2 inp_point,NT inp_pixel_size);
  ~Hot_Pixel();
  inline Point_2 get_center() const;
  bool intersect_left(Segment_2 &seg) const;
  bool intersect_right(Segment_2 &seg) const;
  bool intersect_bot(Segment_2 &seg) const;
  bool intersect_top(Segment_2 &seg) const;
  bool intersect(Segment_data<Rep> &seg) const;
};

// a function for compare two hot pixels for the set of hot pixels
template<class Rep_>
struct hot_pixel_auclidian_cmp
{
  Rep_   _gt;

  bool operator()(const Hot_Pixel<Rep_> *h1,const Hot_Pixel<Rep_> *h2) const;
};

// a function for compare two hot pixels for the set of hot pixels a
// certain segment intersect
template<class Rep_>
struct hot_pixel_dir_cmp
{
  Rep_   _gt;

  bool operator ()(const Hot_Pixel<Rep_> *h1,const Hot_Pixel<Rep_> *h2);
};

template<class base_rep>
class Snap_rounding_traits : public base_rep {

typedef typename base_rep::FT                    NT;
typedef typename base_rep::Point_2               Point_2;
typedef typename base_rep::Segment_2             Segment_2;
typedef typename base_rep::Iso_rectangle_2       Iso_rectangle_2;

public:

Snap_rounding_traits()
  {
    init_angle_appr();
  }

void snap(Point_2 p,NT pixel_size,NT &x,NT &y)
  {
    x = NT(floor((p.x() / pixel_size).to_double())) * pixel_size +
        pixel_size / 2.0;

    y = NT(floor((p.y() / pixel_size).to_double())) * pixel_size +
        pixel_size / 2.0;
  }

double segment_direction(Segment_2 s)
  {
    double x1 = s.source().x().to_double();
    double y1 = s.source().y().to_double();
    double x2 = s.target().x().to_double();
    double y2 = s.target().y().to_double();

    return(atan((y2 - y1)/(x2 - x1)));
  }

Point_2 rotate_point(Point_2 p,NT angle)
  {
    int tranc_angle = int(angle.to_double() * rad_to_deg);
    NT cosine_val = angle_to_sines_appr[90 - tranc_angle],
       sine_val = angle_to_sines_appr[tranc_angle];

    NT x = p.x() * cosine_val - p.y() * sine_val,
       y = p.x() * sine_val + p.y() * cosine_val;

    return(Point_2(x,y));
  }

Iso_rectangle_2 get_bounding_of_min_sum(
                Segment_2 s,
                NT unit_squere,
		NT angle)
  {
    Point_2 ms1,ms2,ms3,ms4,ms5,ms6;// minkowski sum points

    Comparison_result cx = compare_x_2_object()(s.source(),s.target());
    NT x1 = s.source().x(),y1 = s.source().y(),x2 =
       s.target().x(),y2 = s.target().y();

    if(cx == SMALLER) {
      // we use unit_squere instead of unit_squere / 2 in order to
      // find tangency points which are not supported by kd-tree
      ms1 = Point_2(x1 - 0.6 * unit_squere,y1 - 0.6 * unit_squere);
      ms2 = Point_2(x1 - 0.6 * unit_squere,y1 + 0.6 * unit_squere);
      ms3 = Point_2(x1 + 0.6 * unit_squere,y1 - 0.6 * unit_squere);
      ms4 = Point_2(x2 + 0.6 * unit_squere,y2 - 0.6 * unit_squere);
      ms5 = Point_2(x2 + 0.6 * unit_squere,y2 + 0.6 * unit_squere);
      ms6 = Point_2(x2 - 0.6 * unit_squere,y2 + 0.6 * unit_squere);
    } else {
      // we use unit_squere instead of unit_squere / 2 in order to
      // find tangency points which are not supported by kd-tree
      ms1 = Point_2(x1 + 0.6 * unit_squere,y1 - 0.6 * unit_squere);
      ms2 = Point_2(x1 - 0.6 * unit_squere,y1 - 0.6 * unit_squere);
      ms3 = Point_2(x1 + 0.6 * unit_squere,y1 + 0.6 * unit_squere);
      ms4 = Point_2(x2 + 0.6 * unit_squere,y2 + 0.6 * unit_squere);
      ms5 = Point_2(x2 - 0.6 * unit_squere,y2 + 0.6 * unit_squere);
      ms6 = Point_2(x2 - 0.6 * unit_squere,y2 - 0.6 * unit_squere);
    }

    rotate_point(ms1,angle);
    rotate_point(ms2,angle);
    rotate_point(ms3,angle);
    rotate_point(ms4,angle);
    rotate_point(ms5,angle);
    rotate_point(ms6,angle);

    // query
    Point_2 point_left,point_right,point_bot,point_top;

    point_left = small_x_point(ms1,ms2);
    point_left = small_x_point(point_left,ms3);
    point_left = small_x_point(point_left,ms4);
    point_left = small_x_point(point_left,ms5);
    point_left = small_x_point(point_left,ms6);

    point_right = big_x_point(ms1,ms2);
    point_right = big_x_point(point_right,ms3);
    point_right = big_x_point(point_right,ms4);
    point_right = big_x_point(point_right,ms5);
    point_right = big_x_point(point_right,ms6);

    point_bot = small_y_point(ms1,ms2);
    point_bot = small_y_point(point_bot,ms3);
    point_bot = small_y_point(point_bot,ms4);
    point_bot = small_y_point(point_bot,ms5);
    point_bot = small_y_point(point_bot,ms6);

    point_top = big_y_point(ms1,ms2);
    point_top = big_y_point(point_top,ms3);
    point_top = big_y_point(point_top,ms4);
    point_top = big_y_point(point_top,ms5);
    point_top = big_y_point(point_top,ms6);

    Iso_rectangle_2 rec(point_left,point_right,point_bot,point_top);

    return(rec);
  }

 private:
  static const double rad_to_deg = 57.297;
  std::map<const int,NT> angle_to_sines_appr;

Point_2 small_x_point(Point_2 p1,Point_2 p2)
  {
     Comparison_result c = compare_x_2_object()(p1,p2);
     if(c == SMALLER)
       return(p1);
     else
       return(p2);
  }

Point_2 small_y_point(Point_2 p1,Point_2 p2)
  {
     Comparison_result c = compare_y_2_object()(p1,p2);
     if(c == SMALLER)
       return(p1);
     else
       return(p2);
  }

Point_2 big_x_point(Point_2 p1,Point_2 p2)
  {
     Comparison_result c = compare_x_2_object()(p1,p2);
     if(c == SMALLER)
       return(p2);
     else
       return(p1);
  }

Point_2 big_y_point(Point_2 p1,Point_2 p2)
  {
     Comparison_result c = compare_y_2_object()(p1,p2);
     if(c == SMALLER)
       return(p2);
     else
       return(p1);
  }

void init_angle_appr()
  {
    angle_to_sines_appr[0] = NT(0);
    angle_to_sines_appr[1] = NT(115) / NT(6613);
    angle_to_sines_appr[2] = NT(57) / NT(1625);
    angle_to_sines_appr[3] = NT(39) / NT(761);
    angle_to_sines_appr[4] = NT(29) / NT(421);
    angle_to_sines_appr[5] = NT(23) / NT(265);
    angle_to_sines_appr[6] = NT(19) / NT(181);
    angle_to_sines_appr[7] = NT(32) / NT(257);
    angle_to_sines_appr[8] = NT(129) / NT(929);
    angle_to_sines_appr[9] = NT(100) / NT(629);
    angle_to_sines_appr[10] = NT(92) / NT(533);
    angle_to_sines_appr[11] = NT(93) / NT(485);
    angle_to_sines_appr[12] = NT(76) / NT(365);
    angle_to_sines_appr[13] = NT(156) / NT(685);
    angle_to_sines_appr[14] = NT(205) / NT(853);
    angle_to_sines_appr[15] = NT(69) / NT(269);
    angle_to_sines_appr[16] = NT(7) / NT(25);
    angle_to_sines_appr[17] = NT(120) / NT(409);
    angle_to_sines_appr[18] = NT(57) / NT(185);
    angle_to_sines_appr[19] = NT(12) / NT(37);
    angle_to_sines_appr[20] = NT(51) / NT(149);
    angle_to_sines_appr[21] = NT(135) / NT(377);
    angle_to_sines_appr[22] = NT(372) / NT(997);
    angle_to_sines_appr[23] = NT(348) / NT(877);
    angle_to_sines_appr[24] = NT(231) / NT(569);
    angle_to_sines_appr[25] = NT(36) / NT(85);
    angle_to_sines_appr[26] = NT(39) / NT(89);
    angle_to_sines_appr[27] = NT(300) / NT(661);
    angle_to_sines_appr[28] = NT(8) / NT(17);
    angle_to_sines_appr[29] = NT(189) / NT(389);
    angle_to_sines_appr[30] = NT(451) / NT(901);
    angle_to_sines_appr[31] = NT(180) / NT(349);
    angle_to_sines_appr[32] = NT(28) / NT(53);
    angle_to_sines_appr[33] = NT(432) / NT(793);
    angle_to_sines_appr[34] = NT(161) / NT(289);
    angle_to_sines_appr[35] = NT(228) / NT(397);
    angle_to_sines_appr[36] = NT(504) / NT(865);
    angle_to_sines_appr[37] = NT(3) / NT(5);
    angle_to_sines_appr[38] = NT(580) / NT(941);
    angle_to_sines_appr[39] = NT(341) / NT(541);
    angle_to_sines_appr[40] = NT(88) / NT(137);
    angle_to_sines_appr[41] = NT(48) / NT(73);
    angle_to_sines_appr[42] = NT(65) / NT(97);
    angle_to_sines_appr[43] = NT(429) / NT(629);
    angle_to_sines_appr[44] = NT(555) / NT(797);
    angle_to_sines_appr[45] = NT(697) / NT(985);
    angle_to_sines_appr[46] = NT(572) / NT(797);
    angle_to_sines_appr[47] = NT(460) / NT(629);
    angle_to_sines_appr[48] = NT(72) / NT(97);
    angle_to_sines_appr[49] = NT(55) / NT(73);
    angle_to_sines_appr[50] = NT(105) / NT(137);
    angle_to_sines_appr[51] = NT(420) / NT(541);
    angle_to_sines_appr[52] = NT(741) / NT(941);
    angle_to_sines_appr[53] = NT(4) / NT(5);
    angle_to_sines_appr[54] = NT(703) / NT(865);
    angle_to_sines_appr[55] = NT(325) / NT(397);
    angle_to_sines_appr[56] = NT(240) / NT(289);
    angle_to_sines_appr[57] = NT(665) / NT(793);
    angle_to_sines_appr[58] = NT(45) / NT(53);
    angle_to_sines_appr[59] = NT(299) / NT(349);
    angle_to_sines_appr[60] = NT(780) / NT(901);
    angle_to_sines_appr[61] = NT(340) / NT(389);
    angle_to_sines_appr[62] = NT(15) / NT(17);
    angle_to_sines_appr[63] = NT(589) / NT(661);
    angle_to_sines_appr[64] = NT(80) / NT(89);
    angle_to_sines_appr[65] = NT(77) / NT(85);
    angle_to_sines_appr[66] = NT(520) / NT(569);
    angle_to_sines_appr[67] = NT(805) / NT(877);
    angle_to_sines_appr[68] = NT(925) / NT(997);
    angle_to_sines_appr[69] = NT(352) / NT(377);
    angle_to_sines_appr[70] = NT(140) / NT(149);
    angle_to_sines_appr[71] = NT(35) / NT(37);
    angle_to_sines_appr[72] = NT(176) / NT(185);
    angle_to_sines_appr[73] = NT(391) / NT(409);
    angle_to_sines_appr[74] = NT(24) / NT(25);
    angle_to_sines_appr[75] = NT(260) / NT(269);
    angle_to_sines_appr[76] = NT(828) / NT(853);
    angle_to_sines_appr[77] = NT(667) / NT(685);
    angle_to_sines_appr[78] = NT(357) / NT(365);
    angle_to_sines_appr[79] = NT(476) / NT(485);
    angle_to_sines_appr[80] = NT(525) / NT(533);
    angle_to_sines_appr[81] = NT(621) / NT(629);
    angle_to_sines_appr[82] = NT(920) / NT(929);
    angle_to_sines_appr[83] = NT(255) / NT(257);
    angle_to_sines_appr[84] = NT(180) / NT(181);
    angle_to_sines_appr[85] = NT(264) / NT(265);
    angle_to_sines_appr[86] = NT(420) / NT(421);
    angle_to_sines_appr[87] = NT(760) / NT(761);
    angle_to_sines_appr[88] = NT(1624) / NT(1625);
    angle_to_sines_appr[89] = NT(6612) / NT(6613);
    angle_to_sines_appr[90] = NT(1);
  }

};

template<class Rep_>
class Snap_rounding_2 {

typedef CGAL::Arr_segment_traits_2<Rep_ >            Traits;
typedef Rep_                                         Rep;
typedef typename Rep::FT                             NT;
// @@@@ special typedefs for pm
//typedef CGAL::Quotient<CGAL::MP_Float>             NT2;
//typedef CGAL::Cartesian<NT>                        Kernel2;
//typedef CGAL::Arr_segment_traits_2<Kernel2> Traits2;
typedef CGAL::Pm_default_dcel<Traits>      Dcel2;
typedef CGAL::Planar_map_2<Dcel2,Traits>             Planar_map2;
typedef CGAL::Planar_map_with_intersections_2<Planar_map2> Pmwx;
typedef Traits::X_curve                              X_curve2;
// @@@@ end special typedefs for pm
typedef typename Traits::X_curve                     X_curve;
typedef typename Traits::Curve                       Curve;
typedef std::list<X_curve>                           CurveContainer;
typedef typename CurveContainer::iterator            CurveContainerIter;

public:
  friend class Segment_data<Rep>;
  friend class Hot_Pixel<Rep>;
  friend class hot_pixel_dir_cmp<Rep>;

  typedef typename Rep::Segment_2 Segment_2;
  typedef typename Rep::Point_2   Point_2;
  typedef std::list<Point_2> PointList;
  typedef typename std::list<std::list<Point_2> >
             Polylines_container;
  typedef typename Polylines_container::const_iterator Polyline_const_iterator;
  typedef typename Polylines_container::iterator Polyline_iterator;
  typedef typename std::list<typename Rep::Point_2> Points_container;
  typedef typename Points_container::const_iterator Point_const_iterator;
  typedef typename std::list<Segment_2> Segments_container;
  typedef typename Segments_container::const_iterator Segment_const_iterator;
  typedef typename Segments_container::iterator Segment_iterator;

  static bool erase_hp;
  static inline bool get_erase_hp() {return(erase_hp);}

  //! A constructor
  Snap_rounding_2(Segment_const_iterator begin,
                  Segment_const_iterator end,
                  NT inp_pixel_size,bool inp_do_isr = true,
                  int inp_number_of_kd_trees = default_number_of_kd_trees);

  //! A constructor
  Snap_rounding_2(NT inp_pixel_size,bool inp_do_isr = true,
                  int inp_number_of_kd_trees = default_number_of_kd_trees);

  //! A copy constructor
  Snap_rounding_2(const Snap_rounding_2& other);

  //! An operator =
  Snap_rounding_2& operator =(const Snap_rounding_2& other);

   //! Returns a constant iterator to the first input segment.
  inline Segment_const_iterator segments_begin() const {
             return(seg_2_list.begin());}

  //! Returns a constant iterator to the after-the-last of the input segments.
  inline Segment_const_iterator segments_end() const {
    return(seg_2_list.end());}

  //! Returns an iterator to the first input segment.
  inline Segment_iterator segments_begin() {return(seg_2_list.begin());}

  //! Returns an iterator to the after-the-last of the input segments.
  inline Segment_iterator segments_end() {return(seg_2_list.end());}

  //! Returns a constant iterator to the output of the first input segment.
  const Polyline_const_iterator polylines_begin();

  /*! Returns a constant iterator to the after-the-last of the output of
   *  the input segments.
   */
  const Polyline_const_iterator polylines_end();

  //! insert a segment
  bool insert(Segment_2 seg);

  //! The STL standard member function for insertion.
  bool push_back(Segment_2 seg);

  //! Insertion of an iterator range.
  template < class InputIterator >
    int insert(InputIterator first, InputIterator last);
  
  //! Remove a segment
  bool remove(Segment_2 seg);

  //! Remove all segments  .           .
  void clear();

  //! Change the number of kd-trees used for segment-hot pixel queries.
  bool change_number_of_kd_trees(int inp_number_of_kd_trees);

  //! Change the pixel size
  bool change_pixel_size(NT inp_pixel_size);

  /*! Determine whether to apply Iterated Snap Rounding (ISR)
      or Snap Rounding (SR).
   */
  void do_isr(bool inp_do_isr);

  template<class Out>
  void output(Out &o);

private:
  Rep _gt;
  enum Direction {UP_RIGHT,UP_LEFT,DOWN_RIGHT,DOWN_LEFT,UP,DOWN,LEFT,
                  RIGHT,POINT_SEG};

  static Direction seg_dir;
  // the next variable is for lazy evaluation:
  // it determines whether an isr/sr work has
  // to be done (at the beginning, after insertion, etc) 
  bool need_sr;
  static const int default_number_of_kd_trees = 1;

  std::set<Hot_Pixel<Rep> *,hot_pixel_auclidian_cmp<Rep> > hp_set;
  NT pixel_size,min_x,max_x,min_y,max_y;
  Segments_container seg_2_list;
  std::list<Segment_data<Rep> > seg_list;
  std::list<std::list<Point_2> > segments_output_list;
  int number_of_segments,number_of_kd_trees;
  Multiple_kd_tree<Rep,Hot_Pixel<Rep> *> *mul_kd_tree;
  bool wheteher_to_do_isr;
  Pmwx pm;// @@@@

  void find_hot_pixels_and_create_kd_trees();
  void find_intersected_hot_pixels(Segment_data<Rep> &seg,
                         std::set<Hot_Pixel<Rep> *,
                         hot_pixel_dir_cmp<Rep> > &hot_pixels_intersected_set,
                         int &number_of_intersections);

  void reroute_sr(std::set<Hot_Pixel<Rep> *,hot_pixel_dir_cmp<Rep> >
                  &inp_hot_pixels_intersected_set,std::list<Point_2>
                  &seg_output);
  void reroute_isr(std::set<Hot_Pixel<Rep> *,hot_pixel_dir_cmp<Rep> >
                   &inp_hot_pixels_intersected_set,std::list<Point_2>
                   &seg_output,int number_of_intersections,bool first_time);
  void iterate();
  void copy(const Snap_rounding_2<Rep_>& other);
  static inline Direction get_direction() {return(seg_dir);}
  static inline void set_direction(Direction dir) {seg_dir = dir;}
};

// ctor
template<class Rep_>
Segment_data<Rep_>::Segment_data() {}
template<class Rep_>
Segment_data<Rep_>::Segment_data(Point_2 p_inp,Point_2 q_inp) :
                    p(p_inp), q(q_inp) {}

// cctor
template<class Rep_>
Segment_data<Rep_>::Segment_data(const Segment_data& other)
{
  p = other.p;
  q = other.q;
}

template<class Rep_>
inline void Segment_data<Rep_>::set_data(Point_2 inp_p,Point_2 inp_q)
{
  p = inp_p;
  q = inp_q;
}

template<class Rep_>
bool Segment_data<Rep_>::equal(Segment_2 s)
{
  return(s.source() == p && s.target() == q);
}

template<class Rep_>
void Segment_data<Rep_>::determine_direction()
{
  Comparison_result cx = _gt.compare_x_2_object()(p,q);
  Comparison_result cy = _gt.compare_y_2_object()(p,q);

  if(cx == SMALLER) {
   if(cy == SMALLER)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::UP_RIGHT);
    else if(cy == EQUAL)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::RIGHT);
    else
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::DOWN_RIGHT);
  } else if(cx == EQUAL) {
    if(cy == SMALLER)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::UP);
    else if(cy == EQUAL)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::POINT_SEG);
    else
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::DOWN);
  } else {
    if(cy == SMALLER)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::UP_LEFT);
    else if(cy == EQUAL)
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::LEFT);
    else
      Snap_rounding_2<Rep_>::set_direction(Snap_rounding_2<Rep_>::DOWN_LEFT);
  }
}

template<class Rep_>
template<class Out> void Hot_Pixel<Rep_>::draw(Out &o) const
  {
    o << *right_seg;
    o << *left_seg;
    o << *top_seg;
    o << *bot_seg;
  }

  // intersection pixel
template<class Rep_>
Hot_Pixel<Rep_>::Hot_Pixel(Point_2 inp_point,NT inp_pixel_size) :
                           pixel_size(inp_pixel_size)
  {
    NT x,y;
    _gt.snap(inp_point,pixel_size,x,y);

    p = Point_2(x,y);
    p_left = Point_2(x - pixel_size / 2.0,y);
    p_right = Point_2(x + pixel_size / 2.0,y);
    p_down = Point_2(x,y - pixel_size / 2.0);
    p_up = Point_2(x,y + pixel_size / 2.0);

    right_seg = new Segment_2(Point_2(x + pixel_size / 2.0,y -
                              pixel_size / 2.0),
                              Point_2(x + pixel_size / 2.0,y +
                              pixel_size / 2.0));
    left_seg = new Segment_2(Point_2(x - pixel_size / 2.0,y -
                             pixel_size / 2.0),
                             Point_2(x - pixel_size / 2.0,y +
                             pixel_size / 2.0));
    top_seg = new Segment_2(Point_2(x - pixel_size / 2.0,y +
                            pixel_size / 2.0),
                            Point_2(x + pixel_size / 2.0,y +
                            pixel_size / 2.0));
    bot_seg = new Segment_2(Point_2(x - pixel_size / 2.0,y -
                            pixel_size / 2.0),
                            Point_2(x + pixel_size / 2.0,y -
                            pixel_size / 2.0));
  }

template<class Rep_>
Hot_Pixel<Rep_>::~Hot_Pixel()
  {
    if(Snap_rounding_2<Rep_>::get_erase_hp()) {
      delete(right_seg);
      delete(left_seg);
      delete(top_seg);
      delete(bot_seg);
    }
  }

template<class Rep_>
inline Hot_Pixel<Rep_>::Point_2 Hot_Pixel<Rep_>::get_center() const
    {return(p);}

template<class Rep_>
bool Hot_Pixel<Rep_>::intersect_left(Segment_2 &seg) const
  {
    Object result;    
    Point_2 p;
    Segment_2 s;

    result = intersection(seg,*left_seg);//!!!! change: create instance and call its intersection

    if(assign(p,result)) {
      Comparison_result c_p = _gt.compare_y_2_object()(p,p_up);
      Comparison_result c_target = _gt.compare_y_2_object()(seg.target(),p_up);
      Comparison_result c_source = _gt.compare_y_2_object()(seg.source(),p_up);

      return(c_p != EQUAL || Snap_rounding_2<Rep_>::get_direction() ==
             Snap_rounding_2<Rep_>::UP_LEFT && c_source != EQUAL ||
             Snap_rounding_2<Rep_>::get_direction() ==
             Snap_rounding_2<Rep_>::DOWN_RIGHT && c_target != EQUAL);
    } else if(assign(s,result))
      return(true);
    else
      return(false);
  }


template<class Rep_>
bool Hot_Pixel<Rep_>::intersect_right(Segment_2 &seg) const
  {
    Object result;    
    Point_2 p;
    Segment_2 s;
    
    result = intersection(seg,*right_seg);

    if(assign(p,result)) {
      // bottom right point was checked in intersect_bot
      Comparison_result c1 = _gt.compare_y_2_object()(p,p_up);
      Comparison_result c2 = _gt.compare_y_2_object()(p,p_down);
      Comparison_result c3 = _gt.compare_y_2_object()(seg.source(),p_up);
      Comparison_result c4 = _gt.compare_y_2_object()(seg.target(),p_up);

      if(c1 == EQUAL)
        return(Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::UP_RIGHT && c3 != EQUAL ||
               Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::DOWN_LEFT && c4 != EQUAL);
      else if(c2 == EQUAL)
        return(false);// was checked
      else {
        Comparison_result c_target =
             _gt.compare_x_2_object()(p_right,seg.target());
        Comparison_result c_source =
             _gt.compare_x_2_object()(p_right,seg.source());

        return((Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::LEFT ||
                Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::DOWN_LEFT ||
                Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::UP_LEFT) &&
                c_target != EQUAL ||
                (Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::RIGHT ||
                Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::DOWN_RIGHT ||
                Snap_rounding_2<Rep_>::get_direction() ==
                Snap_rounding_2<Rep_>::UP_RIGHT) &&
                c_source != EQUAL);
      }
    } else
      return(false);
  }

template<class Rep_>
bool Hot_Pixel<Rep_>::intersect_bot(Segment_2 &seg) const
  {
    Object result;
    Point_2 p;
    Segment_2 s;

    result = intersection(seg,*bot_seg);

    if(assign(p,result)) {
      Comparison_result c_p = _gt.compare_x_2_object()(p,p_right);
      Comparison_result c_target = _gt.compare_x_2_object()
            (seg.target(),p_right);
      Comparison_result c_source = _gt.compare_x_2_object()
            (seg.source(),p_right);

      return(c_p != EQUAL || Snap_rounding_2<Rep_>::get_direction() ==
             Snap_rounding_2<Rep_>::UP_LEFT && c_target != EQUAL ||
             Snap_rounding_2<Rep_>::get_direction() ==
             Snap_rounding_2<Rep_>::DOWN_RIGHT && c_source != EQUAL);
    } else if(assign(s,result))
      return(true);
    else
      return(false);
  }

template<class Rep_>
bool Hot_Pixel<Rep_>::intersect_top(Segment_2 &seg) const
  {
    Object result;
    Point_2 p;
    Segment_2 s;
    
    result = intersection(seg,*top_seg);

    if(assign(p,result)) {
      // corner points was checked in intersect_bot
      Comparison_result c1 = _gt.compare_x_2_object()(p,p_left);
      Comparison_result c2 = _gt.compare_x_2_object()(p,p_right);
      Comparison_result c3 = _gt.compare_y_2_object()(seg.target(),p_up);
      Comparison_result c4 = _gt.compare_y_2_object()(seg.source(),p_up);

      if(c1 == EQUAL || c2 == EQUAL)
        return(false);// were checked
      else
        return((Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::DOWN ||
               Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::DOWN_LEFT ||
               Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::DOWN_RIGHT) && c3 != EQUAL ||
               (Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::UP ||
               Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::UP_LEFT ||
               Snap_rounding_2<Rep_>::get_direction() ==
               Snap_rounding_2<Rep_>::UP_RIGHT) && c4 != EQUAL);
    } else
    return(false);
  }

template<class Rep_>
bool Hot_Pixel<Rep_>::intersect(Segment_data<Rep_> &seg) const
  {
    Segment_2 s = seg.segment();

    return(intersect_bot(s) || intersect_left(s) || intersect_right(s) ||
           intersect_top(s));
  }

// a function for compare two hot pixels for the set of hot pixels
template<class Rep_>
bool hot_pixel_auclidian_cmp<Rep_>::operator()(const Hot_Pixel<Rep_> *h1,
     const Hot_Pixel<Rep_> *h2) const
  {
    Comparison_result cx = _gt.compare_x_2_object()(
        h1.get_center(),h2.get_center());
    Comparison_result cy = _gt.compare_y_2_object()(
        h1.get_center(),h2.get_center());

    return(cx == SMALLER ||
         cx == EQUAL && cy == SMALLER);
  }

// a function for compare two hot pixels for the set of hot pixels a certain
// segment intersect
template<class Rep_>
bool hot_pixel_dir_cmp<Rep_>::operator ()(const Hot_Pixel<Rep_> *h1,
     const Hot_Pixel<Rep_> *h2) 
{
  Comparison_result cx = _gt.compare_x_2_object()(
      h1->get_center(),h2->get_center());
  Comparison_result cy = _gt.compare_y_2_object()(
      h1->get_center(),h2->get_center());

  return(
     // Point segment intersects only one pixel, thus ignored
    Snap_rounding_2<Rep_>::get_direction() ==
    Snap_rounding_2<Rep_>::UP_RIGHT &&
    (cx == SMALLER || 
    cx == EQUAL && cy == SMALLER) ||
    Snap_rounding_2<Rep_>::get_direction() ==
    Snap_rounding_2<Rep_>::UP_LEFT &&
    (cx == LARGER || 
    cx == EQUAL && cy == SMALLER) ||
    Snap_rounding_2<Rep_>::get_direction() ==
    Snap_rounding_2<Rep_>::DOWN_RIGHT &&
    (cx == SMALLER || 
    cx == EQUAL && cy == LARGER) ||
    Snap_rounding_2<Rep_>::get_direction() ==
    Snap_rounding_2<Rep_>::DOWN_LEFT &&
    (cx == LARGER || 
    cx == EQUAL && cy == LARGER) ||
    Snap_rounding_2<Rep_>::get_direction() == Snap_rounding_2<Rep_>::UP &&
    cy == SMALLER ||
    Snap_rounding_2<Rep_>::get_direction() == Snap_rounding_2<Rep_>::DOWN &&
    cy == LARGER ||
    Snap_rounding_2<Rep_>::get_direction() == Snap_rounding_2<Rep_>::LEFT &&
    cx == LARGER ||
    Snap_rounding_2<Rep_>::get_direction() ==
    Snap_rounding_2<Rep_>::RIGHT &&
    cx == SMALLER);
}

template<class Rep_>
void Snap_rounding_2<Rep_>::find_hot_pixels_and_create_kd_trees()
  {
    Hot_Pixel<Rep_> *hp;
    typename std::list<Segment_data<Rep_> >::iterator iter1;
    Object result;
    Point_2 p;
    std::list<std::pair<Point_2,Hot_Pixel<Rep_> *> > hot_pixels_list;

    list<X_curve> segments;
    for(iter1 = seg_list.begin();iter1 != seg_list.end();++iter1)
      segments.push_back(X_curve(iter1->source(),iter1->target()));

    //    PM pm(new Pm_naive_point_location<PM>);
    // sweep_to_construct_planar_map(segments.begin(), segments.end(), pm);
    std::list<X_curve>  subcurves;

    /*    sweep_to_produce_planar_map_subcurves(segments.begin(), 
					  segments.end(),  
					  traits, 
					  subcurves);*/

    /*// get subcurves with overlapping ********** 
//    CGAL::Sweep_line_tight_2<CurveContainerIter, Traits, Event, SubCurve> sl;
    Sweep_line_2<CurveContainerIter, Traits> sl;
    sl.get_subcurves(segments.begin(), segments.end(),
    std::back_inserter(subcurves));*/

    // get intersection points (with endpoints)
    PointList mypointlist;
    //    CGAL::Sweep_line_tight_2<CurveContainerIter, Traits,
    //                             Event, SubCurve> sl;
    Sweep_line_2<CurveContainerIter, Traits> sl;
    sl.get_intersection_points(segments.begin(), segments.end(),
                             std::back_inserter(mypointlist));

    for(typename std::list<Point_2>::const_iterator
            v_iter = mypointlist.begin();
	v_iter != mypointlist.end();++v_iter) {
      hp = new Hot_Pixel<Rep_>(*v_iter,pixel_size);
      hot_pixels_list.push_back(std::pair<Point_2,Hot_Pixel<Rep_> *>(
				hp->get_center(),hp));
    }



    // @@@@ for ISRS : create new hot pixels




    // create kd multiple tree
    // create simple_list from seg_list
    std::list<Segment_2> simple_seg_list;
    for(typename std::list<Segment_data<Rep_> >::iterator iter =
        seg_list.begin();iter != seg_list.end();++iter)
      simple_seg_list.push_back(Segment_2(iter->source(),iter->target()));

    mul_kd_tree = new Multiple_kd_tree<Rep,Hot_Pixel<Rep> *>(hot_pixels_list,
                  number_of_kd_trees,simple_seg_list);
  }

template<class Rep_>
void Snap_rounding_2<Rep_>::find_intersected_hot_pixels(Segment_data<Rep_>
                    &seg,
                    std::set<Hot_Pixel<Rep_> *,
                    hot_pixel_dir_cmp<Rep_> > &hot_pixels_intersected_set,
                    int &number_of_intersections)
  {
    typename std::list<Hot_Pixel<Rep_> *>::iterator iter;

    hot_pixels_intersected_set.clear();
    seg.determine_direction();
    number_of_intersections = 0;

    std::list<Hot_Pixel<Rep_> *> hot_pixels_list;
    mul_kd_tree->get_intersecting_points(hot_pixels_list,
	   Segment_2(seg.segment()),pixel_size);

    for(iter = hot_pixels_list.begin();iter != hot_pixels_list.end();++iter) {
      if((*iter)->intersect(seg))
        hot_pixels_intersected_set.insert(*iter);
    }

    number_of_intersections = hot_pixels_intersected_set.size();
  }


template<class Rep_>
void Snap_rounding_2<Rep_>::reroute_sr(std::set<Hot_Pixel<Rep_> *,
     hot_pixel_dir_cmp<Rep_> > &inp_hot_pixels_intersected_set,
     std::list<Point_2> &seg_output)
  {
    typename std::set<Hot_Pixel<Rep_> *,
    hot_pixel_dir_cmp<Rep_> >::iterator hot_pixel_iter =
    inp_hot_pixels_intersected_set.begin();
    ++hot_pixel_iter;

    while(hot_pixel_iter != inp_hot_pixels_intersected_set.end()) {
      seg_output.push_back((*hot_pixel_iter)->get_center());
      ++hot_pixel_iter;
    }

  }

template<class Rep_>
void Snap_rounding_2<Rep_>::reroute_isr(std::set<Hot_Pixel<Rep_> *,
   hot_pixel_dir_cmp<Rep_> > &inp_hot_pixels_intersected_set,
   std::list<Point_2> &seg_output,int number_of_intersections,bool first_time)
  {
    typename std::set<Hot_Pixel<Rep_> *,hot_pixel_dir_cmp<Rep_> >::
      iterator hot_pixel_iter,next_hot_pixel_iter,before_last_hot_pixel_iter;
    Segment_data<Rep_> seg;
    std::set<Hot_Pixel<Rep_> *,hot_pixel_dir_cmp<Rep_> >
      hot_pixels_intersected_set;

    if(number_of_intersections > 2 || first_time) {
      before_last_hot_pixel_iter = inp_hot_pixels_intersected_set.end();
      --before_last_hot_pixel_iter;

      for(hot_pixel_iter = inp_hot_pixels_intersected_set.begin();
          hot_pixel_iter != before_last_hot_pixel_iter;++hot_pixel_iter) {
        next_hot_pixel_iter = hot_pixel_iter;
        ++next_hot_pixel_iter;
        seg.set_data((*hot_pixel_iter)->get_center(),
                     (*next_hot_pixel_iter)->get_center());
        seg.determine_direction();
        find_intersected_hot_pixels(seg,hot_pixels_intersected_set,
            number_of_intersections);
        reroute_isr(hot_pixels_intersected_set,seg_output,
            number_of_intersections,false);
      }
    } else {
      // insert second hot pixel
      hot_pixel_iter = inp_hot_pixels_intersected_set.begin();
      ++hot_pixel_iter;
      seg_output.push_back((*hot_pixel_iter)->get_center());
    }
  }


template<class Rep_>
void Snap_rounding_2<Rep_>::iterate()
  {
    std::list<Point_2> seg_output;
    std::set<Hot_Pixel<Rep_> *,hot_pixel_dir_cmp<Rep_> >
      hot_pixels_intersected_set;
    typename std::set<Hot_Pixel<Rep_> *,hot_pixel_dir_cmp<Rep_> >::
      iterator hot_pixel_iter;
    int number_of_intersections;
    Hot_Pixel<Rep_> *hp;
    segments_output_list.clear();
    for(typename std::list<Segment_data<Rep_> >::iterator iter =
        seg_list.begin();iter != seg_list.end();++iter) {
      seg_output.clear();
      iter->determine_direction();
      find_intersected_hot_pixels(*iter,hot_pixels_intersected_set,
        number_of_intersections);
      // hot_pixels_intersected_set must have at least two hot pixels when the
      // segment is not in entirely inside a hot pixel enter first hot pixel
      hot_pixel_iter = hot_pixels_intersected_set.begin();
      if(hot_pixel_iter == hot_pixels_intersected_set.end()) {
        // segment entirely inside a pixel
        hp = new Hot_Pixel<Rep_>(iter->source(),pixel_size);
        seg_output.push_back(hp->get_center());
        erase_hp = true;
        delete(hp);
        erase_hp = false;
      } else {
        seg_output.push_back((*hot_pixel_iter)->get_center());
        if(number_of_intersections > 1) {
          // segments that have at most one intersecting hot pixel are
          // done(it was inserted)
          if(wheteher_to_do_isr)
            reroute_isr(hot_pixels_intersected_set,seg_output,
                        number_of_intersections,true);
          else
            reroute_sr(hot_pixels_intersected_set,seg_output);
	}
      }

      segments_output_list.push_back(seg_output);
    }
  }

template<class Rep_>
Snap_rounding_2<Rep_>::Snap_rounding_2(Segment_const_iterator
  begin,Segment_const_iterator end,
  NT inp_pixel_size,bool inp_do_isr,int inp_number_of_kd_trees)
  {
    // initialize approximation angles map    
    erase_hp = false;
    wheteher_to_do_isr = inp_do_isr;
    pixel_size = inp_pixel_size;
    number_of_segments = 0;
    number_of_kd_trees = inp_number_of_kd_trees;
    need_sr = true;
    // copy segments list
    while(begin != end) {
      seg_list.push_back(Segment_data<Rep_>(begin->source(),
                                            begin->target()));
      seg_2_list.push_back(*begin);
      ++number_of_segments;

      // @@@@ insert to planar map
      //std:: cout << "1st : " << begin->source() <<
      // " snd : " << begin->target() << endl;
      X_curve2 c(begin->source(),begin->target());
      //std::cout << "bbb\n";
      pm.insert(c);
      // @@@@ end of pmwx for here

      ++begin;
    }
  }

template<class Rep_>
void Snap_rounding_2<Rep_>::copy(const Snap_rounding_2<Rep_>& other)
{
    erase_hp = false;
    wheteher_to_do_isr = other.wheteher_to_do_isr;
    pixel_size = other.pixel_size;
    number_of_segments = other.number_of_segments;
    number_of_kd_trees = other.number_of_kd_trees;
    need_sr = true;
    seg_list = other.seg_list;
    seg_2_list = other.seg_2_list;

}

// cctor
template<class Rep_>
Snap_rounding_2<Rep_>::Snap_rounding_2(const Snap_rounding_2<Rep_>& other)
  {
    copy(other);
  }

// operator =
template<class Rep_>
Snap_rounding_2<Rep_>&
Snap_rounding_2<Rep_>::operator =(const Snap_rounding_2<Rep_>& other)
  {
    copy(other);

    return(*this);
  }

template<class Rep_>
Snap_rounding_2<Rep_>::Snap_rounding_2(
    NT inp_pixel_size,bool inp_do_isr,int inp_number_of_kd_trees)
  {
    // initialize approximation angles map
    need_sr = true;
    erase_hp = false;
    wheteher_to_do_isr = inp_do_isr;
    pixel_size = inp_pixel_size;
    number_of_segments = 0;
    number_of_kd_trees = inp_number_of_kd_trees;
  }

template<class Rep_>
bool Snap_rounding_2<Rep_>::insert(Segment_2 seg)
  {
    need_sr = true;
    seg_list.push_back(Segment_data<Rep_>(
					  seg.source(),
                         seg.target()));

    seg_2_list.push_back(seg);
    ++number_of_segments;

    return(true);
  }

template<class Rep_>
bool Snap_rounding_2<Rep_>::push_back(Segment_2 seg)
  {
    return(insert(seg));
  }

template < class Rep_ >
template < class InputIterator >
int
Snap_rounding_2<Rep_>::insert(InputIterator first, InputIterator last)
  {
    need_sr = true;
    int n = 0;
    while(first != last){
      if(insert(*first)){
	n++;
      }
      ++first;
    }
    return n;
  }


template<class Rep_>
const Snap_rounding_2<Rep_>::Polyline_const_iterator
      Snap_rounding_2<Rep_>::polylines_begin()
{
  if(need_sr) {
    need_sr = false;
    find_hot_pixels_and_create_kd_trees();
    iterate();
  }    

  return(segments_output_list.begin());
}

template<class Rep_>
const Snap_rounding_2<Rep_>::Polyline_const_iterator
      Snap_rounding_2<Rep_>::polylines_end()
{
  if(need_sr) {
    need_sr = false;
    find_hot_pixels_and_create_kd_trees();
    iterate();
  }    

  return(segments_output_list.end());
}

template<class Rep_>
bool Snap_rounding_2<Rep_>::remove(Segment_2 seg)
  {
    need_sr = true;
    bool found = false;
    Segment_data<Rep> s;

    for(std::list<Segment_data<Rep> >::iterator i1 = seg_list.begin();
      i1 != seg_list.end();++i1) {
       s = *i1;  
      if(s.equal(seg)) {
        found = true;
        seg_list.erase(i1);
        --number_of_segments;
        break;
      }
    }

    if(found) {
      for(Segment_iterator i2 = seg_2_list.begin();
        i2 != seg_2_list.end();++i2) {
        if(seg == *i2) {
          seg_2_list.erase(i2);
          break;
        }
      }
    }

    return(found);
  }

template<class Rep_>
void Snap_rounding_2<Rep_>::clear()
  { 
    need_sr = true;
    seg_list.clear();
    seg_2_list.clear();
  }

template<class Rep_>
bool Snap_rounding_2<Rep_>::change_number_of_kd_trees(
          int inp_number_of_kd_trees)
  {
    if(inp_number_of_kd_trees > 0) {
      number_of_kd_trees = inp_number_of_kd_trees;
      need_sr = true;
      return(true);
    } else
      return(false);
  }

template<class Rep_>
bool Snap_rounding_2<Rep_>::change_pixel_size(NT inp_pixel_size)
  {
    if(inp_pixel_size > 0) {
      pixel_size = inp_pixel_size;
      need_sr = true;
      return(true);
    } else
      return(false);
  }

template<class Rep_>
void Snap_rounding_2<Rep_>::do_isr(bool inp_do_isr)
  { 
    wheteher_to_do_isr = inp_do_isr;
    need_sr = true;
  }

template<class Rep>
typename Snap_rounding_2<Rep>::Direction Snap_rounding_2<Rep>::seg_dir;

template<class Rep>
bool Snap_rounding_2<Rep>::erase_hp = false;

CGAL_END_NAMESPACE

#endif // CGAL_ISR_2_H
