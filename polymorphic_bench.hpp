#pragma once
#ifndef WTF_POLYMORPHIC_BENCH
#define WTF_POLYMORPHIC_BENCH

#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <typeindex>
#include <map>

class base {
public:
    virtual int foo(int x) const = 0;
};
class d1 : public base {
public:
    d1(int n)
        :n{n}{}
    virtual int foo(int x) const override {
        return x*n;
    }
private:
    int n;
};
class d2 : public base {
public:
    d2(int n)
        :n{n}{}
    virtual int foo(int x) const override {
        return x+n;
    }
private:
    int n;
};
class d3 : public base {
public:
    d3(int n)
        :n{n}{}
    virtual int foo(int x) const override {
        return x-n;
    }
private:
    int n;
};
class d4 : public base {
public:
    d4(int n)
        :n{n}{}
    virtual int foo(int x) const override {
        return x/n;
    }
private:
    int n;
};

template <typename Collection>
Collection fill(std::size_t size){
    Collection coll;
    for (std::size_t i = 0; i < size; i += 4){
        coll.insert(d1(i));
        coll.insert(d2(i+1));
        coll.insert(d3(i+2));
        coll.insert(d4(i+3));
    }
    return coll;
}

template <typename Base>
class ptr_vector {
public:
    template <typename Derived, typename = typename std::enable_if<std::is_base_of<Base, Derived>::value>::type>
    void insert(const Derived& el){
        storage.push_back(ptr(new Derived(el)));
    }

    template <typename Func>
    Func for_each(Func f) const {
        std::for_each(begin(storage), end(storage), [&f](const ptr& el){f(*el);});
        return f;
    }

private:
    using ptr = std::unique_ptr<Base>;
    std::vector<ptr> storage;
};

template <typename Base>
class polymorphic_segment_base {
public:
    virtual ~polymorphic_segment_base(){};
    virtual void insert(const Base& x) = 0;

    virtual void for_each(const std::function<void(const Base&)>& f) const = 0;
};

template <typename Derived, typename Base>
class polymorphic_segment : public polymorphic_segment_base<Base> {
    virtual void insert(const Base& x) override {
        storage.push_back(static_cast<const Derived&>(x));
    }

    virtual void for_each(const std::function<void(const Base&)>& f) const override {
        std::for_each(begin(storage), end(storage), [&f](const Derived& el){f(el);});
    }

private:
    std::vector<Derived> storage;
};


template <typename Base>
class polymorphic_collection {

public:
    template <typename Derived, typename = typename std::enable_if<std::is_base_of<Base, Derived>::value>::type>
    void insert(const Derived& el){
        auto& chunk = chunks[typeid(el)];
        if (!chunk){
            chunk.reset(new polymorphic_segment<Derived, Base>());
        }
        chunk->insert(el);
    }

    template <typename Func>
    Func for_each(Func f) const {
        for (const auto& pair : chunks){
            pair.second->for_each(f);
        }
        return f;
    }

private:
    using segment = polymorphic_segment_base<Base>;
    using ptr = std::unique_ptr<segment>;

    std::map<std::type_index, ptr> chunks;
};

// The following is borrowed from Joaquin M. Lopez Munoz's Bannalia blog.
// Because I had some trouble with my reimplementation.
template<class Base>
class poly_collection_segment_base
{
public:
  virtual ~poly_collection_segment_base(){};

  void insert(const Base& x)
  {
    this->insert_(x);
  }

  template<typename F>
  void for_each(F& f)
  {
    std::size_t s=this->element_size_();
    for(auto it=this->begin_(),end=it+this->size_()*s;it!=end;it+=s){
      f(*reinterpret_cast<Base*>(it));
    }
  }

  template<typename F>
  void for_each(F& f)const
  {
    std::size_t s=this->element_size_();
    for(auto it=this->begin_(),end=it+this->size_()*s;it!=end;it+=s){
      f(*reinterpret_cast<const Base*>(it));
    }
  }

  void shuffle()
  {
    this->shuffle_();
  }

private:
  virtual void insert_(const Base& x)=0;
  virtual char* begin_()=0;
  virtual const char* begin_()const=0;
  virtual std::size_t size_()const=0;
  virtual std::size_t element_size_()const=0;
  virtual void shuffle_()=0;
};

template<class Derived,class Base>
class poly_collection_segment:
  public poly_collection_segment_base<Base>
{
private:
  virtual void insert_(const Base& x)
  {
    store.push_back(static_cast<const Derived&>(x));
  }

  virtual char* begin_()
  {
    return reinterpret_cast<char*>(
      static_cast<Base*>(const_cast<Derived*>(store.data())));
  }

  virtual const char* begin_()const
  {
    return reinterpret_cast<const char*>(
      static_cast<const Base*>(store.data()));
  }

  virtual std::size_t size_()const{return store.size();}
  virtual std::size_t element_size_()const{return sizeof(Derived);}

  virtual void shuffle_()
  {
    std::shuffle(store.begin(),store.end(),std::mt19937(1));
  }

  std::vector<Derived> store;
};

template<class Base>
class poly_collection
{
public:
  template<class Derived>
  void insert(
    const Derived& x,
    typename std::enable_if<std::is_base_of<Base,Derived>::value>::type* =0)
  {
    auto& pchunk=chunks[typeid(x)];
    if(!pchunk)pchunk.reset(new poly_collection_segment<Derived,Base>());
    pchunk->insert(x);
  }

  template<typename F>
  F for_each(F f)
  {
    for(const auto& p:chunks)p.second->for_each(f);
    return std::move(f);
  }

  template<typename F>
  F for_each(F f)const
  {
    for(const auto& p:chunks)
      const_cast<const segment&>(*p.second).for_each(f);
    return std::move(f);
  }

  void shuffle()
  {
    for(const auto& p:chunks)p.second->shuffle();
  }

private:
  typedef poly_collection_segment_base<Base> segment;
  typedef std::unique_ptr<segment>           pointer;

  std::map<std::type_index,pointer> chunks;
};

#endif
