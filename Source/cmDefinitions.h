/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmDefinitions_h
#define cmDefinitions_h

#include <cmConfigure.h>

#include "cmLinkedTree.h"

#include <string>
#include <vector>

#if defined(CMAKE_BUILD_WITH_CMAKE)
#ifdef CMake_HAVE_CXX_UNORDERED_MAP
#include <unordered_map>
#else
#include "cmsys/hash_map.hxx"
#endif
#else
#include <map>
#endif

/** \class cmDefinitions
 * \brief Store a scope of variable definitions for CMake language.
 *
 * This stores the state of variable definitions (set or unset) for
 * one scope.  Sets are always local.  Gets search parent scopes
 * transitively and save results locally.
 */
class cmDefinitions
{
  typedef cmLinkedTree<cmDefinitions>::iterator StackIter;

public:
  static const char* Get(const std::string& key, StackIter begin,
                         StackIter end);

  static void Raise(const std::string& key, StackIter begin, StackIter end);

  static bool HasKey(const std::string& key, StackIter begin, StackIter end);

  /** Set (or unset if null) a value associated with a key.  */
  void Set(const std::string& key, const char* value);

  std::vector<std::string> UnusedKeys() const;

  static std::vector<std::string> ClosureKeys(StackIter begin, StackIter end);

  static cmDefinitions MakeClosure(StackIter begin, StackIter end);

private:
  // String with existence boolean.
  struct Def : public std::string
  {
  private:
    typedef std::string std_string;

  public:
    Def()
      : std_string()
      , Exists(false)
      , Used(false)
    {
    }
    Def(const char* v)
      : std_string(v ? v : "")
      , Exists(v ? true : false)
      , Used(false)
    {
    }
    Def(const std_string& v)
      : std_string(v)
      , Exists(true)
      , Used(false)
    {
    }
    Def(Def const& d)
      : std_string(d)
      , Exists(d.Exists)
      , Used(d.Used)
    {
    }
    bool Exists;
    bool Used;
  };
  static Def NoDef;

#if defined(CMAKE_BUILD_WITH_CMAKE)
#ifdef CMake_HAVE_CXX_UNORDERED_MAP
  typedef std::unordered_map<std::string, Def> MapType;
#else
  typedef cmsys::hash_map<std::string, Def> MapType;
#endif
#else
  typedef std::map<std::string, Def> MapType;
#endif
  MapType Map;

  static Def const& GetInternal(const std::string& key, StackIter begin,
                                StackIter end, bool raise);
};

#endif
