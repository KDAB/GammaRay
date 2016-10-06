/*

    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef KBIHASH_P_H
#define KBIHASH_P_H

#include <QtCore/QHash>
#include <QtCore/QMap>

#include <QtCore/QDebug>

template<typename LeftContainer, typename RightContainer>
class KBiAssociativeContainer;

template<typename LeftContainer, typename RightContainer>
QDebug operator<<(QDebug out, const KBiAssociativeContainer<LeftContainer, RightContainer> &container);

template<typename LeftContainer, typename RightContainer>
QDataStream &operator<<(QDataStream &out, const KBiAssociativeContainer<LeftContainer, RightContainer> &container);

template<typename LeftContainer, typename RightContainer>
QDataStream &operator>>(QDataStream &in, KBiAssociativeContainer<LeftContainer, RightContainer> &container);

template<typename LeftContainer, typename RightContainer>
class KBiAssociativeContainer
{
    // We need to convert from a QHash::iterator or QMap::iterator
    // to a KBiAssociativeContainer::iterator (left or right)
    // Do do that we use this implicit ctor. We partially specialize
    // it for QHash and QMap types.
    // Our iterator inherits from this struct to get the implicit ctor,
    // and this struct must inherit from the QHash or QMap iterator.
    template<typename Container, typename T, typename U>
    struct _iterator_impl_ctor : public Container::iterator {
        _iterator_impl_ctor(typename Container::iterator it);
    };

    template<typename T, typename U>
    struct _iterator_impl_ctor<QHash<T, U>, T, U>  : public QHash<T, U>::iterator {
        /* implicit */ _iterator_impl_ctor(const typename QHash<T, U>::iterator it)
            : QHash<T, U>::iterator(it)
        {

        }
    };

    template<typename T, typename U>
    struct _iterator_impl_ctor<QMap<T, U>, T, U> : public QMap<T, U>::iterator {
        /* implicit */ _iterator_impl_ctor(const typename QMap<T, U>::iterator it)
            : QMap<T, U>::iterator(it)
        {

        }
    };
public:
    typedef typename RightContainer::mapped_type left_type;
    typedef typename LeftContainer::mapped_type right_type;

    template <typename Container>
    class _iterator : public _iterator_impl_ctor<Container, typename Container::key_type, typename Container::mapped_type>
    {
    public:
        explicit inline _iterator(void *data) : Container::iterator(data) {}

        /* implicit */ _iterator(const typename Container::iterator it)
            : _iterator_impl_ctor<Container, typename Container::key_type, typename Container::mapped_type>(it)
        {

        }

        inline const typename Container::mapped_type &value() const
        {
            return Container::iterator::value();
        }
        inline const typename Container::mapped_type &operator*() const
        {
            return Container::iterator::operator*();
        }
        inline const typename Container::mapped_type *operator->() const
        {
            return Container::iterator::operator->();
        }

    private:
#ifndef Q_CC_MSVC
        using Container::iterator::operator*;
        using Container::iterator::operator->;
        using Container::iterator::value;
#endif
    };

    typedef _iterator<LeftContainer>                      left_iterator;
    typedef typename LeftContainer::const_iterator        left_const_iterator;
    typedef _iterator<RightContainer>                     right_iterator;
    typedef typename RightContainer::const_iterator       right_const_iterator;

    inline KBiAssociativeContainer() {}
    inline KBiAssociativeContainer(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        *this = other;
    }

    const KBiAssociativeContainer<LeftContainer, RightContainer> &operator=(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        _leftToRight = other._leftToRight; _rightToLeft = other._rightToLeft; return *this;
    }

    inline bool removeLeft(left_type t)
    {
        const right_type u = _leftToRight.take(t);
        return _rightToLeft.remove(u) != 0;
    }

    inline bool removeRight(right_type u)
    {
        const left_type t = _rightToLeft.take(u);
        return _leftToRight.remove(t) != 0;
    }

    inline right_type takeLeft(left_type t)
    {
        const right_type u = _leftToRight.take(t);
        _rightToLeft.remove(u);
        return u;
    }

    inline left_type takeRight(right_type u)
    {
        const left_type t = _rightToLeft.take(u);
        _leftToRight.remove(t);
        return t;
    }

    inline left_type rightToLeft(right_type u) const
    {
        return _rightToLeft.value(u);
    }

    inline right_type leftToRight(left_type t) const
    {
        return _leftToRight.value(t);
    }

    inline bool leftContains(left_type t) const
    {
        return _leftToRight.contains(t);
    }

    inline bool rightContains(right_type u) const
    {
        return _rightToLeft.contains(u);
    }

    inline int size() const
    {
        return _leftToRight.size();
    }

    inline int count() const
    {
        return _leftToRight.count();
    }

    inline int capacity() const
    {
        return _leftToRight.capacity();
    }

    void reserve(int size)
    {
        _leftToRight.reserve(size); _rightToLeft.reserve(size);
    }

    inline void squeeze()
    {
        _leftToRight.squeeze(); _rightToLeft.squeeze();
    }

    inline void detach()
    {
        _leftToRight.detach(); _rightToLeft.detach();
    }

    inline bool isDetached() const
    {
        return _leftToRight.isDetached();
    }

    inline void setSharable(bool sharable)
    {
        _leftToRight.setSharable(sharable); _rightToLeft.setSharable(sharable);
    }

    inline bool isSharedWith(const KBiAssociativeContainer<RightContainer, LeftContainer> &other) const
    {
        return _leftToRight.isSharedWith(other._leftToRight) && _rightToLeft.isSharedWith(other._leftToRight);
    }

    void clear()
    {
        _leftToRight.clear(); _rightToLeft.clear();
    }

    QList<left_type> leftValues() const
    {
        return _leftToRight.keys();
    }

    QList<right_type> rightValues() const
    {
        return _rightToLeft.keys();
    }

    right_iterator eraseRight(right_iterator it)
    {
        Q_ASSERT(it != rightEnd());
        _leftToRight.remove(it.value());
        return _rightToLeft.erase(it);
    }

    left_iterator eraseLeft(left_iterator it)
    {
        Q_ASSERT(it != leftEnd());
        _rightToLeft.remove(it.value());
        return _leftToRight.erase(it);
    }

    left_iterator findLeft(left_type t)
    {
        return _leftToRight.find(t);
    }

    left_const_iterator findLeft(left_type t) const
    {
        return _leftToRight.find(t);
    }

    left_const_iterator constFindLeft(left_type t) const
    {
        return _leftToRight.constFind(t);
    }

    right_iterator findRight(right_type u)
    {
        return _rightToLeft.find(u);
    }

    right_const_iterator findRight(right_type u) const
    {
        return _rightToLeft.find(u);
    }

    right_const_iterator constFindRight(right_type u) const
    {
        return _rightToLeft.find(u);
    }

    left_iterator insert(left_type t, right_type u)
    {
        // biHash.insert(5, 7); // creates 5->7 in _leftToRight and 7->5 in _rightToLeft
        // biHash.insert(5, 9); // replaces 5->7 with 5->9 in _leftToRight and inserts 9->5 in _rightToLeft.
        // The 7->5 in _rightToLeft would be dangling, so we remove it before insertion.

        // This means we need to hash u and t up to twice each. Could probably be done better using QHashNode.

        if (_leftToRight.contains(t)) {
            _rightToLeft.remove(_leftToRight.take(t));
        }
        if (_rightToLeft.contains(u)) {
            _leftToRight.remove(_rightToLeft.take(u));
        }

        _rightToLeft.insert(u, t);
        return _leftToRight.insert(t, u);
    }

    KBiAssociativeContainer<LeftContainer, RightContainer> &intersect(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        typename KBiAssociativeContainer<RightContainer, LeftContainer>::left_iterator it = leftBegin();
        while (it != leftEnd()) {
            if (!other.leftContains(it.key())) {
                it = eraseLeft(it);
            } else {
                ++it;
            }
        }
        return *this;
    }

    KBiAssociativeContainer<LeftContainer, RightContainer> &subtract(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        typename KBiAssociativeContainer<RightContainer, LeftContainer>::left_iterator it = leftBegin();
        while (it != leftEnd()) {
            if (other._leftToRight.contains(it.key())) {
                it = eraseLeft(it);
            } else {
                ++it;
            }
        }
        return *this;
    }

    KBiAssociativeContainer<LeftContainer, RightContainer> &unite(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        typename LeftContainer::const_iterator it = other._leftToRight.constBegin();
        const typename LeftContainer::const_iterator end = other._leftToRight.constEnd();
        while (it != end) {
            const left_type key = it.key();
            if (!_leftToRight.contains(key)) {
                insert(key, it.value());
            }
            ++it;
        }
        return *this;
    }

    void updateRight(left_iterator it, right_type u)
    {
        Q_ASSERT(it != leftEnd());
        const left_type key = it.key();
        _rightToLeft.remove(_leftToRight.value(key));
        _leftToRight[key] = u;
        _rightToLeft[u] = key;
    }

    void updateLeft(right_iterator it, left_type t)
    {
        Q_ASSERT(it != rightEnd());
        const right_type key = it.key();
        _leftToRight.remove(_rightToLeft.value(key));
        _rightToLeft[key] = t;
        _leftToRight[t] = key;
    }

    inline bool isEmpty() const
    {
        return _leftToRight.isEmpty();
    }

    const right_type operator[](const left_type &t) const
    {
        return _leftToRight.operator[](t);
    }

    bool operator==(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        return _leftToRight.operator == (other._leftToRight);
    }

    bool operator!=(const KBiAssociativeContainer<LeftContainer, RightContainer> &other)
    {
        return _leftToRight.operator != (other._leftToRight);
    }

    left_iterator toLeftIterator(right_iterator it) const
    {
        Q_ASSERT(it != rightEnd());
        return _leftToRight.find(it.value());
    }

    right_iterator toRightIterator(left_iterator it) const
    {
        Q_ASSERT(it != leftEnd());
        return _rightToLeft.find(it.value());
    }

    inline left_iterator leftBegin()
    {
        return _leftToRight.begin();
    }

    inline left_iterator leftEnd()
    {
        return _leftToRight.end();
    }

    inline left_const_iterator leftBegin() const
    {
        return _leftToRight.begin();
    }

    inline left_const_iterator leftEnd() const
    {
        return _leftToRight.end();
    }

    inline left_const_iterator leftConstBegin() const
    {
        return _leftToRight.constBegin();
    }

    inline left_const_iterator leftConstEnd() const
    {
        return _leftToRight.constEnd();
    }

    inline right_iterator rightBegin()
    {
        return _rightToLeft.begin();
    }

    inline right_iterator rightEnd()
    {
        return _rightToLeft.end();
    }

    inline right_const_iterator rightBegin() const
    {
        return _rightToLeft.begin();
    }

    inline right_const_iterator rightEnd() const
    {
        return _rightToLeft.end();
    }
    inline right_const_iterator rightConstBegin() const
    {
        return _rightToLeft.constBegin();
    }

    inline right_const_iterator rightConstEnd() const
    {
        return _rightToLeft.constEnd();
    }

    static KBiAssociativeContainer<LeftContainer, RightContainer> fromHash(const QHash<left_type, right_type> &hash)
    {
        KBiAssociativeContainer<LeftContainer, RightContainer> container;
        typename QHash<left_type, right_type>::const_iterator it = hash.constBegin();
        const typename QHash<left_type, right_type>::const_iterator end = hash.constEnd();
        for (; it != end; ++it) {
            container.insert(it.key(), it.value());
        }
        return container;
    }

    static KBiAssociativeContainer<LeftContainer, RightContainer> fromMap(const QMap<left_type, right_type> &hash)
    {
        KBiAssociativeContainer<LeftContainer, RightContainer> container;
        typename QMap<left_type, right_type>::const_iterator it = hash.constBegin();
        const typename QMap<left_type, right_type>::const_iterator end = hash.constEnd();
        for (; it != end; ++it) {
            container.insert(it.key(), it.value());
        }
        return container;
    }

    friend QDataStream &operator<< <LeftContainer, RightContainer>(QDataStream &out, const KBiAssociativeContainer<LeftContainer, RightContainer> &bihash);
    friend QDataStream &operator>> <LeftContainer, RightContainer>(QDataStream &in, KBiAssociativeContainer<LeftContainer, RightContainer> &biHash);
    friend QDebug operator<< <LeftContainer, RightContainer>(QDebug out, const KBiAssociativeContainer<LeftContainer, RightContainer> &biHash);
protected:
    LeftContainer _leftToRight;
    RightContainer _rightToLeft;
};

template<typename LeftContainer, typename RightContainer>
QDataStream &operator<<(QDataStream &out, const KBiAssociativeContainer<LeftContainer, RightContainer> &container)
{
    return out << container._leftToRight;
}

template<typename LeftContainer, typename RightContainer>
QDataStream &operator>>(QDataStream &in, KBiAssociativeContainer<LeftContainer, RightContainer> &container)
{
    LeftContainer leftToRight;
    in >> leftToRight;
    typename LeftContainer::const_iterator it = leftToRight.constBegin();
    const typename LeftContainer::const_iterator end = leftToRight.constEnd();
    for (; it != end; ++it) {
        container.insert(it.key(), it.value());
    }

    return in;
}

template<typename Container, typename T, typename U>
struct _containerType {
    operator const char *();
};

template<typename T, typename U>
struct _containerType<QHash<T, U>, T, U> {
    operator const char *()
    {
        return "QHash";
    }
};

template<typename T, typename U>
struct _containerType<QMap<T, U>, T, U> {
    operator const char *()
    {
        return "QMap";
    }
};

template<typename Container>
static const char *containerType()
{
    return _containerType<Container, typename Container::key_type, typename Container::mapped_type>();
}

template<typename LeftContainer, typename RightContainer>
QDebug operator<<(QDebug out, const KBiAssociativeContainer<LeftContainer, RightContainer> &container)
{
    typename KBiAssociativeContainer<LeftContainer, RightContainer>::left_const_iterator it = container.leftConstBegin();

    const typename KBiAssociativeContainer<LeftContainer, RightContainer>::left_const_iterator end = container.leftConstEnd();
    out.nospace() << "KBiAssociativeContainer<" << containerType<LeftContainer>() << ", " << containerType<RightContainer>() << ">" << "(";
    for (; it != end; ++it) {
        out << "(" << it.key() << " <=> " << it.value() << ") ";
    }

    out << ")";
    return out;
}

/**
 * @brief KBiHash provides a bi-directional hash container
 *
 * @note This class is designed to make mapping easier in proxy model implementations.
 *
 * @todo Figure out whether to discard this and use boost::bimap instead, submit it Qt or keep it here and make more direct use of QHashNode.
 */
template <typename T, typename U>
struct KBiHash : public KBiAssociativeContainer<QHash<T, U>, QHash<U, T> > {
    KBiHash()
        : KBiAssociativeContainer<QHash<T, U>, QHash<U, T> > ()
    {

    }

    KBiHash(const KBiAssociativeContainer<QHash<T, U>, QHash<U, T> > &container)
        : KBiAssociativeContainer<QHash<T, U>, QHash<U, T> > (container)
    {

    }
};

template<typename T, typename U>
QDebug operator<<(QDebug out, const KBiHash<T, U> &biHash)
{
    typename KBiHash<T, U>::left_const_iterator it = biHash.leftConstBegin();

    const typename KBiHash<T, U>::left_const_iterator end = biHash.leftConstEnd();
    out.nospace() << "KBiHash(";
    for (; it != end; ++it) {
        out << "(" << it.key() << " <=> " << it.value() << ") ";
    }

    out << ")";
    return out;
}

template <typename T, typename U>
struct KHash2Map : public KBiAssociativeContainer<QHash<T, U>, QMap<U, T> > {
    KHash2Map()
        : KBiAssociativeContainer<QHash<T, U>, QMap<U, T> > ()
    {

    }

    KHash2Map(const KBiAssociativeContainer<QHash<T, U>, QMap<U, T> > &container)
        : KBiAssociativeContainer<QHash<T, U>, QMap<U, T> > (container)
    {

    }

    typename KBiAssociativeContainer<QHash<T, U>, QMap<U, T> >::right_iterator rightLowerBound(const U &key)
    {
        return this->_rightToLeft.lowerBound(key);
    }

    typename KBiAssociativeContainer<QHash<T, U>, QMap<U, T> >::right_const_iterator rightLowerBound(const U &key) const
    {
        return this->_rightToLeft.lowerBound(key);
    }

    typename KBiAssociativeContainer<QHash<T, U>, QMap<U, T> >::right_iterator rightUpperBound(const U &key)
    {
        return this->_rightToLeft.upperBound(key);
    }

    typename KBiAssociativeContainer<QHash<T, U>, QMap<U, T> >::right_const_iterator rightUpperBound(const U &key) const
    {
        return this->_rightToLeft.upperBound(key);
    }
};

template<typename T, typename U>
QDebug operator<<(QDebug out, const KHash2Map<T, U> &container)
{
    typename KHash2Map<T, U>::left_const_iterator it = container.leftConstBegin();

    const typename KHash2Map<T, U>::left_const_iterator end = container.leftConstEnd();
    out.nospace() << "KHash2Map(";
    for (; it != end; ++it) {
        out << "(" << it.key() << " <=> " << it.value() << ") ";
    }

    out << ")";
    return out;
}

#endif
