#pragma once

#include "resource.h"
#include "stdafx.h"

// ��Ⱥ���ñ�����˵�������
class cluster_configuration
{
public:

	int replica_mode; //1��������3�����ɾ��
	int replica;
	int erasure_code_k;
	int erasure_code_m;

	int lb_cnt;
	int rgw_cnt;
	int osd_cnt;
	int osd_rgw_mixcnt;
	int osd_lb_mixcnt;

	int osd_per_srv;

	int bonding_mode;

	bool divided_network;

	int network_speed;

	cluster_configuration()
	{
		reset();
	}
	~cluster_configuration() {};

	bool check_valid();
	void reset();
};

typedef struct simulator_result
{
	double lb_pub_downlink;
	double lb_pub_uplink;

	double rgw_pub_downlink;
	double rgw_pub_uplink;

	double osd_pub_downlink;
	double osd_pub_uplink;
	double osd_cluster_uplink;
	double osd_cluster_downlink;

	double osd_rgw_pub_downlink;
	double osd_rgw_pub_uplink;
	double osd_rgw_cluster_downlink;
	double osd_rgw_cluster_uplink;

	double osd_lb_pub_downlink;
	double osd_lb_pub_uplink;
	double osd_lb_cluster_downlink;
	double osd_lb_cluster_uplink;

	double osd_downlink;
	double osd_uplink;
	double osd_rgw_downlink;
	double osd_rgw_uplink;
	double osd_lb_downlink;
	double osd_lb_uplink;

	double ethernet_adapter_limit;
	double osd_limit;
	double cluster_max_flow;
};

bool cluster_configuration::check_valid()
{
	bool flag = true;

	if (replica > 16 || replica < 1)flag = false;
	if (erasure_code_k > 16 || erasure_code_k < 1)flag = false;;
	if (erasure_code_m > 4 || erasure_code_m < 0)flag = false;

	return flag;
}

void cluster_configuration::reset()
{
	// Ĭ��Ϊ������
	replica_mode = 1;
	replica = 3;
	erasure_code_k = 0;
	erasure_code_m = 0;

	lb_cnt = 0;
	rgw_cnt = 0;
	osd_cnt = 0;
	osd_rgw_mixcnt = 0;
	osd_lb_mixcnt = 0;

	osd_per_srv = 0;

	// Ĭ��BOND1��˫�����룬���
	bonding_mode = 1;
	divided_network = true;
	network_speed = 10;
}