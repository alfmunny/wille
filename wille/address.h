#ifndef __WILLE_ADDRESS_H__
#define __WILLE_ADDRESS_H__

#include <memory>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <map>


namespace wille {

class IPAddress;

class Address {

public:
    typedef std::shared_ptr<Address> ptr;

    static Address::ptr Create(const sockaddr* addr, socklen_t addrlen);
    static bool Lookup(std::vector<Address::ptr>& result, const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    static Address::ptr LookupAny(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);

    static bool GetInterfaceAddresses(std::multimap<std::string, std::pair<Address::ptr, uint32_t>>& result, int family = AF_INET);

    static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>>& result,
            const std::string& iface, int family = AF_INET);


    virtual ~Address() = default;

    int getFamily() const;

    virtual const sockaddr* getAddr()  const = 0;
    virtual sockaddr* getAddr() = 0;
    virtual socklen_t getAddrLen() const = 0;
    virtual std::ostream& insert(std::ostream& os) const = 0;

    std::string toString() const;

    bool operator<(const Address& rhs) const;
    bool operator=(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};
}

#endif
