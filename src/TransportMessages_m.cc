
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "TransportMessages_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(TransportMsg)

TransportMsg::TransportMsg(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

TransportMsg::TransportMsg(const TransportMsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

TransportMsg::~TransportMsg()
{
}

TransportMsg& TransportMsg::operator=(const TransportMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void TransportMsg::copy(const TransportMsg& other)
{
    this->sourceAddress = other.sourceAddress;
    this->broadcastMsg = other.broadcastMsg;
    this->destinationAddress = other.destinationAddress;
    this->headerSize = other.headerSize;
    this->payloadSize = other.payloadSize;
}

void TransportMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sourceAddress);
    doParsimPacking(b,this->broadcastMsg);
    doParsimPacking(b,this->destinationAddress);
    doParsimPacking(b,this->headerSize);
    doParsimPacking(b,this->payloadSize);
}

void TransportMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sourceAddress);
    doParsimUnpacking(b,this->broadcastMsg);
    doParsimUnpacking(b,this->destinationAddress);
    doParsimUnpacking(b,this->headerSize);
    doParsimUnpacking(b,this->payloadSize);
}

const char * TransportMsg::getSourceAddress() const
{
    return this->sourceAddress.c_str();
}

void TransportMsg::setSourceAddress(const char * sourceAddress)
{
    this->sourceAddress = sourceAddress;
}

bool TransportMsg::getBroadcastMsg() const
{
    return this->broadcastMsg;
}

void TransportMsg::setBroadcastMsg(bool broadcastMsg)
{
    this->broadcastMsg = broadcastMsg;
}

const char * TransportMsg::getDestinationAddress() const
{
    return this->destinationAddress.c_str();
}

void TransportMsg::setDestinationAddress(const char * destinationAddress)
{
    this->destinationAddress = destinationAddress;
}

int TransportMsg::getHeaderSize() const
{
    return this->headerSize;
}

void TransportMsg::setHeaderSize(int headerSize)
{
    this->headerSize = headerSize;
}

int TransportMsg::getPayloadSize() const
{
    return this->payloadSize;
}

void TransportMsg::setPayloadSize(int payloadSize)
{
    this->payloadSize = payloadSize;
}

class TransportMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sourceAddress,
        FIELD_broadcastMsg,
        FIELD_destinationAddress,
        FIELD_headerSize,
        FIELD_payloadSize,
    };
  public:
    TransportMsgDescriptor();
    virtual ~TransportMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TransportMsgDescriptor)

TransportMsgDescriptor::TransportMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TransportMsg)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

TransportMsgDescriptor::~TransportMsgDescriptor()
{
    delete[] propertyNames;
}

bool TransportMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TransportMsg *>(obj)!=nullptr;
}

const char **TransportMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TransportMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TransportMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int TransportMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sourceAddress
        FD_ISEDITABLE,    // FIELD_broadcastMsg
        FD_ISEDITABLE,    // FIELD_destinationAddress
        FD_ISEDITABLE,    // FIELD_headerSize
        FD_ISEDITABLE,    // FIELD_payloadSize
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *TransportMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sourceAddress",
        "broadcastMsg",
        "destinationAddress",
        "headerSize",
        "payloadSize",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int TransportMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sourceAddress") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "broadcastMsg") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destinationAddress") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "headerSize") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "payloadSize") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *TransportMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_sourceAddress
        "bool",    // FIELD_broadcastMsg
        "string",    // FIELD_destinationAddress
        "int",    // FIELD_headerSize
        "int",    // FIELD_payloadSize
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **TransportMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TransportMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TransportMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TransportMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TransportMsg'", field);
    }
}

const char *TransportMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TransportMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: return oppstring2string(pp->getSourceAddress());
        case FIELD_broadcastMsg: return bool2string(pp->getBroadcastMsg());
        case FIELD_destinationAddress: return oppstring2string(pp->getDestinationAddress());
        case FIELD_headerSize: return long2string(pp->getHeaderSize());
        case FIELD_payloadSize: return long2string(pp->getPayloadSize());
        default: return "";
    }
}

void TransportMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: pp->setSourceAddress((value)); break;
        case FIELD_broadcastMsg: pp->setBroadcastMsg(string2bool(value)); break;
        case FIELD_destinationAddress: pp->setDestinationAddress((value)); break;
        case FIELD_headerSize: pp->setHeaderSize(string2long(value)); break;
        case FIELD_payloadSize: pp->setPayloadSize(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransportMsg'", field);
    }
}

omnetpp::cValue TransportMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: return pp->getSourceAddress();
        case FIELD_broadcastMsg: return pp->getBroadcastMsg();
        case FIELD_destinationAddress: return pp->getDestinationAddress();
        case FIELD_headerSize: return pp->getHeaderSize();
        case FIELD_payloadSize: return pp->getPayloadSize();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TransportMsg' as cValue -- field index out of range?", field);
    }
}

void TransportMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: pp->setSourceAddress(value.stringValue()); break;
        case FIELD_broadcastMsg: pp->setBroadcastMsg(value.boolValue()); break;
        case FIELD_destinationAddress: pp->setDestinationAddress(value.stringValue()); break;
        case FIELD_headerSize: pp->setHeaderSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_payloadSize: pp->setPayloadSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransportMsg'", field);
    }
}

const char *TransportMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TransportMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TransportMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TransportMsg *pp = omnetpp::fromAnyPtr<TransportMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TransportMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp
