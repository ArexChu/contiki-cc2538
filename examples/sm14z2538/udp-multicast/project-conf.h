int8_t cc2538_rf_channel_set(uint8_t channel);
#define MULTICHAN_CONF_SET_CHANNEL cc2538_rf_channel_set
int cc2538_rf_read_rssi(void);
#define MULTICHAN_CONF_READ_RSSI cc2538_rf_read_rssi
#define w_memcpy memcpy /* Needed to make multichan.c compile */
#define MULTICHAN_CONF_NR_CHANNELS    16
#define MULTICHAN_CONF_CHANNEL_OFFSET 11


#define NETSTACK_RADIO_MAX_PAYLOAD_LEN 125

#define NETSTACK_AES_KEY "thingsquare mist"

#undef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON            1

/* Allow the device to join two multicast groups. */
#undef UIP_CONF_DS6_MADDR_NBU
#define UIP_CONF_DS6_MADDR_NBU 2

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS                 20

#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU               20

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  1000
#undef UIP_CONF_TCP_MSS
#define UIP_CONF_TCP_MSS         1000
