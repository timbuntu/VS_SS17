/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Store_H
#define Store_H

#include <thrift/TDispatchProcessor.h>
#include "Store_types.h"



class StoreIf {
 public:
  virtual ~StoreIf() {}
  virtual int32_t getPrice(const std::string& item) = 0;
  virtual bool order(const std::string& item, const int16_t amount) = 0;
  virtual void getReceipt(std::vector<Order> & _return) = 0;
};

class StoreIfFactory {
 public:
  typedef StoreIf Handler;

  virtual ~StoreIfFactory() {}

  virtual StoreIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(StoreIf* /* handler */) = 0;
};

class StoreIfSingletonFactory : virtual public StoreIfFactory {
 public:
  StoreIfSingletonFactory(const boost::shared_ptr<StoreIf>& iface) : iface_(iface) {}
  virtual ~StoreIfSingletonFactory() {}

  virtual StoreIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(StoreIf* /* handler */) {}

 protected:
  boost::shared_ptr<StoreIf> iface_;
};

class StoreNull : virtual public StoreIf {
 public:
  virtual ~StoreNull() {}
  int32_t getPrice(const std::string& /* item */) {
    int32_t _return = 0;
    return _return;
  }
  bool order(const std::string& /* item */, const int16_t /* amount */) {
    bool _return = false;
    return _return;
  }
  void getReceipt(std::vector<Order> & /* _return */) {
    return;
  }
};

typedef struct _Store_getPrice_args__isset {
  _Store_getPrice_args__isset() : item(false) {}
  bool item;
} _Store_getPrice_args__isset;

class Store_getPrice_args {
 public:

  Store_getPrice_args() : item() {
  }

  virtual ~Store_getPrice_args() throw() {}

  std::string item;

  _Store_getPrice_args__isset __isset;

  void __set_item(const std::string& val) {
    item = val;
  }

  bool operator == (const Store_getPrice_args & rhs) const
  {
    if (!(item == rhs.item))
      return false;
    return true;
  }
  bool operator != (const Store_getPrice_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_getPrice_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Store_getPrice_pargs {
 public:


  virtual ~Store_getPrice_pargs() throw() {}

  const std::string* item;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_getPrice_result__isset {
  _Store_getPrice_result__isset() : success(false) {}
  bool success;
} _Store_getPrice_result__isset;

class Store_getPrice_result {
 public:

  Store_getPrice_result() : success(0) {
  }

  virtual ~Store_getPrice_result() throw() {}

  int32_t success;

  _Store_getPrice_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const Store_getPrice_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Store_getPrice_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_getPrice_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_getPrice_presult__isset {
  _Store_getPrice_presult__isset() : success(false) {}
  bool success;
} _Store_getPrice_presult__isset;

class Store_getPrice_presult {
 public:


  virtual ~Store_getPrice_presult() throw() {}

  int32_t* success;

  _Store_getPrice_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Store_order_args__isset {
  _Store_order_args__isset() : item(false), amount(false) {}
  bool item;
  bool amount;
} _Store_order_args__isset;

class Store_order_args {
 public:

  Store_order_args() : item(), amount(0) {
  }

  virtual ~Store_order_args() throw() {}

  std::string item;
  int16_t amount;

  _Store_order_args__isset __isset;

  void __set_item(const std::string& val) {
    item = val;
  }

  void __set_amount(const int16_t val) {
    amount = val;
  }

  bool operator == (const Store_order_args & rhs) const
  {
    if (!(item == rhs.item))
      return false;
    if (!(amount == rhs.amount))
      return false;
    return true;
  }
  bool operator != (const Store_order_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_order_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Store_order_pargs {
 public:


  virtual ~Store_order_pargs() throw() {}

  const std::string* item;
  const int16_t* amount;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_order_result__isset {
  _Store_order_result__isset() : success(false) {}
  bool success;
} _Store_order_result__isset;

class Store_order_result {
 public:

  Store_order_result() : success(0) {
  }

  virtual ~Store_order_result() throw() {}

  bool success;

  _Store_order_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const Store_order_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Store_order_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_order_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_order_presult__isset {
  _Store_order_presult__isset() : success(false) {}
  bool success;
} _Store_order_presult__isset;

class Store_order_presult {
 public:


  virtual ~Store_order_presult() throw() {}

  bool* success;

  _Store_order_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class Store_getReceipt_args {
 public:

  Store_getReceipt_args() {
  }

  virtual ~Store_getReceipt_args() throw() {}


  bool operator == (const Store_getReceipt_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Store_getReceipt_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_getReceipt_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Store_getReceipt_pargs {
 public:


  virtual ~Store_getReceipt_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_getReceipt_result__isset {
  _Store_getReceipt_result__isset() : success(false) {}
  bool success;
} _Store_getReceipt_result__isset;

class Store_getReceipt_result {
 public:

  Store_getReceipt_result() {
  }

  virtual ~Store_getReceipt_result() throw() {}

  std::vector<Order>  success;

  _Store_getReceipt_result__isset __isset;

  void __set_success(const std::vector<Order> & val) {
    success = val;
  }

  bool operator == (const Store_getReceipt_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Store_getReceipt_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Store_getReceipt_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Store_getReceipt_presult__isset {
  _Store_getReceipt_presult__isset() : success(false) {}
  bool success;
} _Store_getReceipt_presult__isset;

class Store_getReceipt_presult {
 public:


  virtual ~Store_getReceipt_presult() throw() {}

  std::vector<Order> * success;

  _Store_getReceipt_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class StoreClient : virtual public StoreIf {
 public:
  StoreClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  StoreClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  int32_t getPrice(const std::string& item);
  void send_getPrice(const std::string& item);
  int32_t recv_getPrice();
  bool order(const std::string& item, const int16_t amount);
  void send_order(const std::string& item, const int16_t amount);
  bool recv_order();
  void getReceipt(std::vector<Order> & _return);
  void send_getReceipt();
  void recv_getReceipt(std::vector<Order> & _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class StoreProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<StoreIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (StoreProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_getPrice(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_order(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_getReceipt(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  StoreProcessor(boost::shared_ptr<StoreIf> iface) :
    iface_(iface) {
    processMap_["getPrice"] = &StoreProcessor::process_getPrice;
    processMap_["order"] = &StoreProcessor::process_order;
    processMap_["getReceipt"] = &StoreProcessor::process_getReceipt;
  }

  virtual ~StoreProcessor() {}
};

class StoreProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  StoreProcessorFactory(const ::boost::shared_ptr< StoreIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< StoreIfFactory > handlerFactory_;
};

class StoreMultiface : virtual public StoreIf {
 public:
  StoreMultiface(std::vector<boost::shared_ptr<StoreIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~StoreMultiface() {}
 protected:
  std::vector<boost::shared_ptr<StoreIf> > ifaces_;
  StoreMultiface() {}
  void add(boost::shared_ptr<StoreIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  int32_t getPrice(const std::string& item) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->getPrice(item);
    }
    return ifaces_[i]->getPrice(item);
  }

  bool order(const std::string& item, const int16_t amount) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->order(item, amount);
    }
    return ifaces_[i]->order(item, amount);
  }

  void getReceipt(std::vector<Order> & _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->getReceipt(_return);
    }
    ifaces_[i]->getReceipt(_return);
    return;
  }

};



#endif