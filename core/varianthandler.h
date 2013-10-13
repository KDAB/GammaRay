#ifndef GAMMARAY_VARIANTHANDLER_H
#define GAMMARAY_VARIANTHANDLER_H

#include "include/gammaray_core_export.h"

#include <QVariant>

namespace GammaRay {

/** Variant conversion functions, extendable by plugins. */
namespace VariantHandler
{
  /** @internal */
  template <typename RetT> struct Converter
  {
    virtual RetT operator() (const QVariant &v) = 0;
  };

  /** @internal */
  template <typename RetT, typename InputT, typename FuncT> struct ConverterImpl : public Converter<RetT>
  {
    explicit inline ConverterImpl(FuncT converter) : f(converter) {}
    /*override*/ inline RetT operator() (const QVariant &v) { return f(v.value<InputT>()); }
    FuncT f;
  };

  /**
   * Returns a human readable string version of the QVariant value.
   * Converts to the variant type and prints the string value accordingly.
   * @param value is a QVariant.
   *
   * @return a QString containing the human readable string.
   */
  GAMMARAY_CORE_EXPORT QString displayString(const QVariant &value);

  /**
   * Returns a value representing @p value in a itemview decoration role.
   * @param value is a QVariant.
   *
   * @return a QVariant itemview decoration role.
   */
  GAMMARAY_CORE_EXPORT QVariant decoration(const QVariant &value);

  /**
   * Register a string conversion functions for a variant type.
   * @internal
   */
  GAMMARAY_CORE_EXPORT void registerStringConverter(int type, Converter<QString> *converter);

  /**
   * Register a string conversion function for a variant type.
   * @tparam T The type for which to use this converter function.
   */
  template <typename T, typename FuncT>
  inline void registerStringConverter(FuncT f)
  {
    Converter<QString> *converter = new ConverterImpl<QString, T, FuncT>(f);
    registerStringConverter(qMetaTypeId<T>(), converter);
  }
}

}

#endif // GAMMARAY_VARIANTHANDLER_H
