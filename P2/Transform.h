#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <fstream>
#include <sstream>

/**
 * @namespace Transform
 * All function and object used to transforming data are in this namespace.
 */
namespace Transform {

  // for transformation using object
  std::string buf;
  std::stringstream ss;

  /**
   * @namespace tr_detail
   * The detailed function for user interface are in this namespace.
   */
  namespace tr_detail {
    /**
     * @brief The base of transformation function.
     * @param arg The last argument for the transformation.
     */
    template <typename T>
    inline void transforming(T &&arg)
    {
      ss >> arg;
      ss.str("");
      ss.clear();
    }

    /**
     * @brief The detail implementation of the transformation function,
     *        use recursive template parameter pack to transform the argument.
     *
     * @tparam Arg The type of the head argument in the parameter pack.
     * @tparam T The type of the parameter pack.
     * @param first The head argument in the parameter pack.
     * @param args The remain argument in the parameter pack.
     */
    template <typename Arg, typename... T>
    inline void transforming(Arg &&first, T &&...args)
    {
      ss >> first;
      transforming(std::forward<decltype(args)>(args)...);
    }
  }    // namespace tr_detail

  // user transforming function
  /**
   * @brief The interface for user to transform data from stringstream/
   *
   * @tparam T The type of the parameter pack.
   * @param args The argument used to passed in detail implemented function.
   */
  template <typename... T>
  void trans_input(T &&...args)
  {
    ss << buf;
    tr_detail::transforming(std::forward<decltype(args)>(args)...);
  }
}    // namespace Transform

#endif