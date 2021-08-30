#include "gateway/stock_level2/l2_gateway.h"
#include "object.h"

SMARTQUANT_NAMESPACE_START

L2Td::L2Td(L2Gateway* gateway)
{}

L2Td::~L2Td()
{}

void L2Td::init()
{
    connect_status_ = true;
    login_status_ = true;
}

bool L2Td::login()
{
    return true;
}

bool L2Td::req_settlement_confirm()
{
    return true;
}

bool L2Td::req_auth()
{
    return true;
}

int L2Td::send_order(const OrderRequest& req)
{
    return -1;
}

bool L2Td::cancel_order(const CancelRequest& req)
{
    return true;
}

SMARTQUANT_NAMESPACE_END