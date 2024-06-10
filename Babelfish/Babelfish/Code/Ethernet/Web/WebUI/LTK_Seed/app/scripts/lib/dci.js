(function(){
var True = 1, False = 0;
var m = ['',99,'uint8','DCI_IOT_',999,'uint16','string8',31,'PWR_CONV_',-1,'bool','Output ',12,'uint32','Device ','Name',10,'Phase ','Network ','Configuration','Product ','Battery ','ENABLE','Enable','ENDIAN_',15,'U8()','Seconds','Ethernet ','Certificate ','IPV4_BIG_','Enable ','PWR_SUM_','Assembly ',20,'Server ','Voltage ',32,13,'Proxy ','DCI_SNTP_','Address ','DCI_LOG_','Trusted ','Input ','Conflict ','DCI_PHY1_','Eth Port ','Detection ','Status','float','IP filter ','Multicast ','DCI_PRODUCT_','MDNS Server ','PowerConverter ','Outlet ','\n0 - ','BATSYS_','\n1 - ','Disable ','Bypass ','Generation ','PROXY_','1 Voltage','NAME','address ','Voltage','\n0 = ','\n2 = ','DCI_ETH_','Control','Method ','= 0)','DCI_USER_','Time','will be ','Maximum ','login ','status ','DCI_TRUSTED_','of IP ','server ','from NV ','Simple ','used to ','word','device ','CADENCE','Group ','Language ','type','DCI_AUDIT_','Duplex ','Auto Negotiation ','Number','Audit ','Log for ','DCI_DEVICE_','DCI_MDNS_','\n3 = ','Serial ','value ','\n1 = ','2 Voltage','STATE','Firmware ','3 Voltage','The IP ','Mode','\nTrue ','RTC Time','Code','SERVER_','of Bypass ','HOST_ADDRESS()','SNTP server ','Frequency','The SNTP ','EVENT_','OUT_PH_','LOAD','DCI_NV_','or Domain ','Type','Version ','1_VOLT','IP_ADDRESS','IP Address','TIME','being ','used on ','Port','OUTLSYS_','STAT','Modbus ','an insecure ','Test Command','Start ','protocol ','and enabling ','IOT Connection ','it can ','lead to ','security ','risk','the network.','DCI_STATIC_','Stored ','ACD Conflicted ',300,1800,'select ','Failed ','in the ','configuration.','Event ','power ','string','server','UPDATE_','after ','\n2 - ','taken ','Detected ','certificate ','name','DCI_IP_','= Description ','ACD_CONFLICTED_','DCI_ACTIVE_','Present ','Reset ','SERVER1_','IN_1_PH_','1 Phase ','of Input ','1, Phase ','The active ','1 Frequency','Status ','IN_2_PH_','1 Autonegotiate ','Disable','Broadcast ',101,'settings: ','State','\n0x00000000: ','Log test ','Address','App Firmware ','Processor ','FIRM_VER_','Ver Num','Web Firmware ','USERNAME','Logged ','user name','MAC_ADDRESS()','Auto-Negotiation ','1 Speed ','IP address ','Only used ','the Ethernet ','DCI_MODBUS_','TCP_ENABLE','when Auto-Negotiate ','Bacnet ','SUBNET_','MASK','Subnet ','Mask','subnet ','of Control ','DEFAULT_','GATEWAY','Timeout','which ','Allocation ','is from ','Default ','Gateway','default ','DCI_MAX_','gateway ','used in ','the NV ','a user ','before ','origin ','IP_ENABLE','IP Enable','DIP switch ','is disabled. ','DCI_WEB_','list  ','Control ','to this ','DCI_SIMPLE_','WORD','factory ','reset. ','ADDRESS_','application ','ALLOCATION_','If enabled ','ASCII','will generate ','SERVICE_','LWM2M ','Service ','Discovery ','Upgrade ','Assigned ','1 Local ','Domain ','User assigned ','FULL_DUPLEX_','ACTUAL','Allocation','1 IP Address','allocate ','32bit ','an IP ','PORT','Number. ','1 Service ','Port Number','Open Source ','License','(MAE)','TIMEOUT','User account ','Inactivity ','Average ','for all ','phases','timeout ','(Range ','2_VOLT','list Modbus/TCP ','Address: ','3_VOLT','Restore ','DCI_TIME_','FORMAT','Reboot ','Power','reset, ','Current','Efficiency','Validity ','(hardcoded ','SNTP Operation ','192.168.1.254). ','DHCP. ','Full address ','Memory. ','\n3 - ',210,'TYPE','Upper ','SNTP Update ','Part Number','Nominal ','three ','SNTP Retry ','Voltage, ','Pole1','Temperatue','Run Time ','to Empty','Remaining ','Capactiy','Percent ','Load','MODE','PowerSummary ','octets ','SNTP Active ','Alarm ','DCI_UNIX_','From Bypass ','EPOCH_','epoch ','To Bypass ','Time from ','1 Status','2 Status','STATUS','1 Full ','3 Status','Test Result','and lower ','BAT_ADTST_','TST_W_','SERVER','octet ','selected ','Level','by the ','Password ','dip switch ','Actual ','IOT hub ','GUID','DEVICE_','received ','from PX ','white','mode. ','CRED()','\nVal ','Preferred ','Setting','No Conflict ','Connection ','IOT Hub ','Common ','of the ','parameters ','RATE','Update ','Rate','rate from ','= Full ','to cloud',120,600,'SeeMe ','LINK_SPEED_','\nFalse ','1s cadence, ','StoreMe ','= Half ','Duplex','Selects ','or Disable ','conflict ','DCI_VENDOR_','Vendor ','vendor ','CODE','AUTONEG_','\nValue ','with Filter '],
b = 'bcdfghjklmpqrstuvwxyzABCDEFGHIJKLMPQRSTUVWXYZ', c=b.length,
i = 0,k = function(x) {
    if (!x--) return '';
    return k(Math.floor(x/c))+b[x%c];
}
while (m.length) {
    m[k(++i)] = m.shift(); 
}
// Using 'with' below to make things MUCH smaller, i.e. a=m[0], b=m[1] ... aa=m[52] ...
with (m) {
// DCI array contains object with keys: define, pid, r, w, format, datatype, count, name, desc, units, min, max, enum, enum_desc, modbus
// [modbus] array contains object with keys: reg, length, start
var _ = function (a, menum, modbus, mbitfield){
    var obj = {define:a[0],pid:a[1],r:a[2],w:a[3],format:a[4],datatype:a[5],count:a[6],name:a[7],desc:a[8],units:a[9],export:a[10],import:a[11]};
    if (menum) {
        if (Array.isArray(menum[0])) {
            obj.enum=menum[0];
            obj.enum_desc=menum[1]||menum[0];
        } else {
            obj.min=menum[0];
            obj.max=menum[1];
        }
    }
    if (mbitfield) {
        if (Array.isArray(mbitfield[0])) {
            obj.bitfield=mbitfield[0];
            obj.bitfield_desc=mbitfield[1];
        } else {
            obj.min=mbitfield[0];
            obj.max=mbitfield[1];
        }
    }
    if (modbus) {
        var b=obj.modbus=[];
        for (var i=0;i<modbus.length;i++) {
            var c=modbus[i];
            b[i]={reg:c[0],length:c[1],start:c[2]||0};
        }
    }
    return obj;
};
window.DCI=[
_([bc+kq+'SELECT',0,7,k,b,h,1,bd+fA+'Select','Select '+fD+'link speed. '+fC+fG+gm+'\nValid values: 10, 100','Mbs',False,False],[[v,100],['10 Mbps speed','100 Mbps speed']],[[90,2]]),
_([bc+kq+gP,1,7,g,b,h,1,bd+fA+'Actual',jI+H+'link speed.','Mbs',False,False],0,[[91,2]]),
_([bc+gM+B,2,7,k,b,p,1,bd+jv+cf+C,kw+'the duplex '+jR+fC+fG+gm+cz+kj+cf+kr+ku+kv,b,False,False],[False,m],[[92,1]]),
_([bc+gM+gP,3,7,g,b,p,1,bd+jv+cf+bg,jI+'duplex '+jR+cz+kj+cf+kr+ku+kv,b,False,False],[False,m],[[93,1]]),
_([bc+kD+B,4,7,k,b,p,1,bd+fd+C,kw+fz+'of link speed and duplex. '+cz+'= Enabled',b,False,False],[False,m],[[94,1]]),
_([bc+kD+cu,5,7,g,b,d,1,bd+fd+fk,'Active state '+kb+fz+'behavior. '+kE+'= State '+bC+'Link Inactive '+cs+cg+'in Progress '+bD+cg+dx+'(Defafult used) '+cp+cg+'of Duplex '+dx+'(speed ok) \n4 = Auto Negotiation Success \n5 = '+cg+'Disabled \n6 = Port Disabled',b,False,False],0,[[95,1]]),
_([bc+'PORT_ENABLE',6,7,k,b,p,1,bd+'1 Enabled','This parameter can disable '+fD+'Port. Effectively disabling the port. '+cz+'= Enable Port \nFalse = Disable '+cX,b,False,False],[False,m],[[96,1]]),
_([dL+'ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS',14,7,g,b,d,1,fb+fP+bR+W+fU+bG+fV+gl+'or Ethernet',fb+fP+bR+W+fU+bG+fV+gl+'or Ethernet. \n1= DIP Switch \n2=Ethernet',b,False,False],[[1,2],['DIP Switch','Ethernet']]),
_([dL+gx+gz+'METHOD_'+ju,E,7,g,b,d,1,'Current '+bG+bR+gQ,'Shows the present method '+bV+gS+gU+hr+br+ht+hC+hE+bt+hF+dG+hG+dH+bT+hH+hI+hL+hR+jg+bT+jy+jC+jD+jF+jH+'setting.',b,False,False],[0,3],[[77,1]]),
_([dL+gx+gz+'METHOD',16,7,k,b,d,1,bG+bR+gQ,'Allows to set the method '+bV+gS+gU+hr+br+ht+hC+hE+bt+hF+dG+hG+dH+bT+hH+hI+hL+hR+jg+bT+jy+jC+jD+jF+jH+'setting. \n(Critical '+kc+'- Care must be '+dH+'while switching to different method)',b,False,False],[[0,1,2,3],['Statically Hardcoded','DHCP Allocated','Taken From NV','DIP Switch Configured']],[[76,1]]),
_([bE+'ACD_ENABLE',24,7,k,b,p,1,'ACD Enable',W+bb+bf+'enable. ACD provides protection from duplicate IP addresses on the network.',b,False,False],[False,m]),
_([bE+'ACD_CONFLICT_'+cu,25,7,g,b,d,1,'ACD Status',W+bb+bf+'State. '+jT+dM+bC+jW+dI+cs+bb+dI+'Defending '+bD+bb+dI+'Retreated',b,False,False]),
_([bE+dP+cu,26,7,k,b,d,1,dt+bg,W+bb+bf+'Status. The state of ACD activity when last '+ky+'was detected. '+jT+dM+bC+jW+cs+'Probe IPV4 Address '+bD+'Ongoing '+bf+cp+'Semi-Active Probing',b,False,False]),
_([bE+dP+'MAC',27,7,k,fy,d,6,dt+'MAC',W+bb+bf+'conflicted '+bX+'MAC address. The source MAC address from the header '+kb+jM+H+'packet '+fT+'was sent by a device reporting a conflict.',b,False,False]),
_([bE+dP+'ARP_PDU',28,7,k,b,d,28,dt+'PDU',W+bb+bf+'raw ARP PDU when '+ky+'was detected.',b,False,False]),
_([bl+'SERIAL_NUM',29,7,k,b,s,1,cq+ch,gT+t+cq+gW+hc,b,False,False],0,[[0,4]]),
_([kz+bz,30,7,g,b,j,r,kA+u,z+kB+'name character '+dC,b,False,False],0,[[2,r]]),
_([bl+kC,k,7,g,b,h,1,z+cB,z+'code numerical',b,False,False],0,[[8,2]]),
_([bl+'CODE_ASCII',S,7,g,b,j,7,'ASCII '+z+cB,z+'code character '+dC,b,False,False],0,[[9,7]]),
_([kz+'URL',33,7,g,b,j,14,kA+'URL',z+kB+'URL',b,False,False],0,[[T,14]]),
_([bl+bz,34,7,g,b,j,S,z+u,z+u,b,False,False],0,[[P,S]]),
_(['DCI_MODEL_'+bz,35,7,g,b,j,v,'Model '+u,z+'model '+dK,b,False,False],0,[[36,v]]),
_([bI+'APP_NAME',36,7,k,b,j,S,gI+u,gL+dK,b,False,False],0,[[41,S]]),
_([fE+'TCP_COMM_'+hd,37,7,k,b,h,1,db+'TCP Com '+fS,'Communication '+hl+'for Modbus TCP.  0 = Disable.','milliseconds',False,False],0,[[fh,2]]),
_(['DCI_MULTICAST_'+B,38,7,k,b,s,1,bk+K+ff,bk+fj+kE+': Description \n0x00000010: '+bk+K+fl+bk+bu+kF+'PerfectFilter \n0x00000004: '+bk+bu+kF+'HashTable \n0x00000404: '+bk+bu+'with Filter_PerfectHashTable',b,False,False],0,[[97,4]]),
_(['DCI_BROADCAST_'+B,39,7,k,b,s,1,fg+K+ff,fg+K+bu+fj+fl+'BroadcastFramesReception_Enable \n0x00000020: BroadcastFramesReception_Disable',b,False,False],0,[[c,4]]),
_(['DCI_APP_'+fs+'NUM',2010,7,g,b,s,1,fq+ft,fq+cQ+ch,b,False,False],0,[[57,4]]),
_([gp+fs+'NUM',2011,7,g,b,s,1,fu+ft,fu+cQ+ch,b,False,False]),
_(['DCI_ETHERNET_MAC_ADDRESS',2028,7,k,fy,d,6,H+'MAC Address','Unique MAC Address assigned '+gs+'device.(MAE)',b,False,False]),
_([dQ+cS,2030,7,g,J+D+F,d,4,dR+H+cT,dY+fB+cV+cW+dq,'IP',False,False],0,[[78,4]]),
_([dQ+fI+fJ,2031,7,g,J+D+F,d,4,dR+H+fK+fL,dY+fM+'mask IP '+bA+cV+cW+dq,'IP',False,False],0,[[80,4]]),
_([dQ+fQ+fR,2032,7,g,J+D+F,d,4,dR+H+fW+fX,dY+fY+gb+fB+cV+cW+dq,'IP',False,False],0,[[82,4]]),
_([dr+cS,2033,7,k,J+D+F,d,4,ds+H+cT,cx+bA+gc+gd+bA+dw+dz,'IP',False,False],0,[[84,4]]),
_([dr+fI+fJ,2034,7,k,J+D+F,d,4,ds+H+fK+fL,cx+fM+'mask used '+dy+'NV address '+dw+dz,'IP',False,False],0,[[86,4]]),
_([dr+fQ+fR,2035,7,k,J+D+F,d,4,ds+H+fW+fX,cx+fY+gb+gc+gd+bA+dw+dz,'IP',False,False],0,[[88,4]]),
_([gp+'ACCESS_PASSWORD_WAIVER_LEVEL',10000,0,c,'WAIVER()',d,1,jG+'Waiver '+jE,'Any level lower than or equal '+gs+cr+'does not need a password',b,False,False],[[0,7,E,k,c],['This can be accessd without authentication','This is a Role having readonly access','This is an Operator Role having Product control privileges','This is an Engineer Role having Product configuration privileges','This is an Administrator having all privileges']]),
_([gt+'APP_CONTROL_'+gu,10014,7,k,b,h,1,bU+'App Control '+bW,bU+gy+'control '+bW,b,m,m],0,[[103,2]]),
_([gt+'APP_STATUS_'+gu,10015,7,g,b,h,1,bU+'App status '+bW,bU+gy+bP+bW,b,m,m],0,[[104,2]]),
_(['DCI_LTK_VER_ASCII',10016,7,g,b,j,19,'LTK Rev '+gB,'LTK revision '+gB,b,False,False],0,[[105,19]]),
_(['DCI_FW_UPGRADE_'+jd,10017,7,c,b,d,1,cv+gH+cy,cv+gH+cy,b,False,False],[0,2]),
_([bI+'WEB_FW_'+bz,10024,7,g,b,j,S,gI+u,gL+dK,b,False,False]),
_([bl+'PROC_NAME',10025,7,g,b,j,S,z+u,z+u,b,False,False]),
_([bl+'PROC_SERIAL_NUM',10026,7,c,b,s,1,cq+ch,gT+t+cq+gW+hc,b,False,False]),
_([bI+'INACTIVITY_'+hd,10027,7,c,b,h,1,hf+hg+'timeout',hf+hg+hl+'In seconds',G,False,False],[900,dv]),
_(['DCI_HTTP_'+B,10028,7,c,b,p,1,'HTTP','HTTP is '+dc+dg+dh+dk+dl+dm+dp,b,False,False],[[0,1],['Disable','Enable']]),
_([bQ+'IP_WHITELIST',10029,7,c,J+D+F,d,r,Y+'IP Address filter white list','Enter 3 comma separated IPs or range. \n Eg. 192.168.1.25 (Fixed IP), 166.99.170.255 '+hm+'166.99.170.x) , 172.48.255.255 '+hm+': 172.48.x.x). \nWhere \'X\' '+cr+'range from 1-255.','IP',False,False]),
_([bQ+'IP_MODBUS_'+fF,10030,7,c,b,p,1,Y+bj+hq+C,Y+bj+hq+C,b,False,False]),
_([jk+jm+cU,10032,7,c,b,s,1,'Set Time','32 bit '+jp+jr+'RTC',b,m,m],0,[[125,4]]),
_([hu+'OFFSET',10033,7,c,b,'sint16',1,'Timezone','Timezone '+cr+bK+'set with offset. E.g GMT+530 '+cr+'would be -330',b,m,m],[[660,km,540,480,420,360,du,270,240,hJ,180,kl,60,0,-60,-120,-180,-210,-240,-300,-330,-345,-360,-390,-420,-480,-540,-570,-600,-660,-720,-780],['GMT-11:00','GMT-10:00','GMT-09:00','GMT-08:00','GMT-07:00','GMT-06:00','GMT-05:00','GMT-04:30','GMT-04:00','GMT-03:30','GMT-03:00','GMT-02:00','GMT-01:00','GMT/UTC','GMT+01:00','GMT+02:00','GMT+03:00','GMT+03:30','GMT+04:00','GMT+05:00','GMT+05:30','GMT+05:45','GMT+06:00','GMT+06:30','GMT+07:00','GMT+08:00','GMT+09:00','GMT+09:30','GMT+10:00','GMT+11:00','GMT+12:00','GMT+13:00']]),
_(['DCI_DATE_'+hv,10034,7,c,b,d,1,'Date Format','Date format values '+bC+'mm/dd/yyyy '+cs+'dd/mm/yyyy '+bD+'yyyy-mm-dd '+cp+'dd mm yyyy',b,m,m],[[0,1,2,3],['mm/dd/yyyy','dd/mm/yyyy','yyyy-mm-dd','dd mm yyyy']]),
_([hu+hv,10035,7,c,b,d,1,'Time Format','Time format can be '+br+'12Hrs \n1- 24Hrs',b,m,m],[[0,1],['12Hrs (AM/PM)','24Hrs']]),
_(['DCI_REST_RESET_COMMAND',10036,7,c,b,d,1,hw+'or Reset Device',hw+t+'is soft '+hy+bX+bK+dB+'down and '+dB+'up. Factory '+dS+'is hard '+hy+'all non-volatile data will be wiped out. Please refresh browser once you save it.',b,False,False],[[1,2],['Reboot Device','Factory Reset']]),
_([cl+'CERT_VALID',10037,7,g,b,d,1,t+I+hB+bg,t+I+hB+fb+br+I+'Invalid '+bt+I+'Valid',b,False,False],[[0,1],['Certificate Invalid','Certificate Valid']]),
_([V+gD+B,10038,7,c,b,d,1,hD+C,hD+C,b,False,False],[0,1]),
_([V+cC+'1',10039,7,c,cE,j,40,cF+'1',cH+Q+'1 IP address '+cM+u,b,False,False]),
_([V+cC+'2',10040,7,c,cE,j,40,cF+'2',cH+Q+'2 IP address '+cM+u,b,False,False]),
_([V+cC+'3',10041,7,c,cE,j,40,cF+'3',cH+Q+'3 IP address '+cM+u,b,False,False]),
_([V+dE+cU,10042,7,c,b,s,1,hM+bJ,hM+bJ,b,False,False],[15000,86400000]),
_([V+'RETRY_'+cU,10043,7,c,b,h,1,hS+bJ,hS+'time will be doubled when no '+bS+'is responding. It will increased upto 10 times of retry time which user has configured.',b,False,False],[3000,20000]),
_([V+'ACTIVE_'+jB,10044,7,g,b,d,1,jh+Q+bg,jh+Q+bg,b,False,False],[0,3]),
_([jk+jm+'TIME_64_BIT',10045,7,c,b,s,2,'Set Time 64bit;Seconds;Microseconds','64 bit (32 bit in seconds and 32 bit in microseconds) '+jp+jr+'RTC','S,uS',False,False]),
_([f+'CONNECT',10046,7,c,b,p,1,'IOT Enable or Disable',gr+'button to connect or disconnect to IOT(connect = 1, disconnect '+bH,b,False,False],0,[[185,1]]),
_([f+ju,10047,7,g,b,p,1,dj+bg,'Show the current '+bP+'with the IOT like connected/disconnected(connected = 1, disconnected '+bH,b,False,False],0,[[186,1]]),
_([f+bx+B,10048,7,c,b,p,1,U+C,'This will be used to use or bypass '+U+bS+'settings (Proxy '+K+'= 1, Proxy '+bu+bH,b,False,False]),
_([f+bx+jB,10049,7,c,b,j,P,U+Q+fp,U+bS+'address',b,False,False]),
_([f+bx+gV,10050,7,c,b,h,1,U+Q+cX,U+bS+'port',b,False,False]),
_([f+bx+fv,10051,7,c,b,j,21,U+'Username','User name '+bV+bM+dy+jJ+dD,b,False,False]),
_([f+bx+'PASSWORD',10052,7,c,jS,j,21,U+'Password',jG+bV+bM+dy+jJ+dD,b,False,False]),
_([f+jL+jK,10053,7,c,b,j,37,t+jK,t+'GUID received '+jP+jQ,b,False,False]),
_([f+jL+'PROFILE',10054,7,c,b,j,37,t+'Profile',t+'Profile '+jM+jP+jQ,b,False,False]),
_([f+'CONN_STRING',10055,7,c,jS,j,154,jY+Q+jX+dC,jX+'string to access '+jY+dD,b,False,False]),
_([f+dE+kd,10056,7,g,b,d,1,'IOT Data '+kf+kg,'Data update '+kh+bX+kk,b,False,False],[[5,v,E,P,25,30],['5 seconds','10 seconds','15 seconds','20 seconds','25 seconds','30 seconds']],[[187,1]]),
_([f+'CADENCE_'+dE+kd,10057,7,g,b,d,1,'IOT Cadence '+kf+kg,'Cadence data update '+kh+bX+kk,b,False,False],[[5,v,E,P,25,30],['5 seconds','10 seconds','15 seconds','20 seconds','25 seconds','30 seconds']],[[188,1]]),
_(['DCI_GROUP0_'+bY,10058,7,c,b,bh,1,bZ+'0 Cadence','10s cadence, '+kp+cc,G,False,False],[[m,0,1,5,v,E,P],['Stop publishing','Immediate publishing','Cadence 1s','Cadence 5s','Cadence 10s','Cadence 15s','Cadence 20s']]),
_(['DCI_GROUP1_'+bY,10059,7,c,b,bh,1,bZ+'1 Cadence','60s cadence, '+kt+'on interval '+cc,G,False,False],[[m,v,60,kl,du,500,km],['Stop publishing','Cadence 10s','Cadence 60s','Cadence 120s','Cadence 300s','Cadence 500s','Cadence 600s']]),
_(['DCI_GROUP2_'+bY,10060,7,c,b,bh,1,bZ+'2 Cadence',ks+kp+cc,G,False,False],[[m,0,1,5,v,E,P],['Stop publishing','Immediate publishing','Cadence 1s','Cadence 5s','Cadence 10s','Cadence 15s','Cadence 20s']]),
_(['DCI_GROUP3_'+bY,10061,7,c,b,bh,1,bZ+'3 Cadence',ks+kt+cc,G,False,False],[[m,0,1,5,v,E,P],['Stop publishing','Immediate publishing','Cadence 1s','Cadence 5s','Cadence 10s','Cadence 15s','Cadence 20s']]),
_([X+'TEST_1',10062,7,c,b,bh,1,'Log test','Test variable for logging test',b,False,False],0,[[133,4]]),
_(['DCI_RESET_CAUSE',10063,7,g,b,'byte',1,dS+'Cause','Provides the cause of a reset. \nBit '+dM+bC+'Power Up \n1 = Wakeup '+bD+'Watchdog '+cp+'Software \n4 = '+dS+'Pin \n5 = Brown Out \n6 = Undefined',b,False,False],0,[[71,1]],[[0,1,2,3,4,5,6],['Standard power-up','Wakeup from sleep','Watchdog reset','Software - internally','Microcontroller reset pin','Brown out reset','Reset cause undefined']]),
_(['DCI_LOGGING_'+cI+kC,10064,7,g,b,d,1,'Logging Event',b,b,False,False],[[0,1,2,3,4,5,6,7,8,9,v,11,r,T,14,E,16,17,18,19,P,21,22,23,24,25,26,27,28,29,30,k,S,33,34,35,fh,102],['No Event','Device Restart','Factory Reset','Image upgrade started','Image upgrade complete','Checksum failure','Signature verification failure','Version rollback failure','NV data restore fail','NV param write fail','NV parameter erase fail','NV erase all parameters fail','Self user password change','Other user password change','User created','User deleted','Admin user logged event','User logged in event','User logout','User Locked event','Reset default admin user','Reset all user accounts','Data Log Memory Clear','Event Log Memory Clear','Audit-Power Log Memory Clear','Audit-Fw-Update Log Memory Clear','Audit-User Log Memory Clear','Audit-Config Log Memory Clear','Image upgrade started over IOT','Image upgrade completed over IOT','Image upgrade aborted/cancelled over IOT','Image upgrade failed over IOT due to bad parameter','Checksum failure over IOT','Signature verification failure over IOT','Version rollback failure over IOT','IOT FUS request rejected on product side','Test event-101','Test event-102']]),
_([cl+cu,10065,7,g,b,d,1,t+fk,b,b,False,False],[[0,1],['Operational','Fault']]),
_([X+cI+'TEST_101',10066,7,c,b,p,1,dA+fm+'101',b,b,False,False],0,[[135,1]]),
_([X+cI+'TEST_102',10067,7,c,b,p,1,dA+fm+'102',b,b,False,False],0,[[136,1]]),
_([cL+'MEM_ID',10068,7,g,b,d,1,'NV Memory ID',b,b,False,False],[[0,1],['FRAM','Undefined']]),
_([cL+'PARAMETER_ADDRESS',10069,7,g,b,s,1,'NV Parameter '+fp,b,b,False,False]),
_([cL+'FAIL_OPERATION',10070,7,g,b,d,1,'NV Operation',b,b,False,False],[[0,1,2,3,4,5],['Initilization','Read Parameter','Read Checksum','Write Parameter','Erase Parameter','Erase All Parameters']]),
_([X+'PROC_IMAGE_ID',10071,7,c,b,d,1,fr+'and Image',fr+'and Image ID',b,False,False],[[0,1,2],['Processor Main and Image APPLICATION','Processor Main and Image WEB','Processor Main and Image LANGUAGE']]),
_([X+'FIRMWARE_VERSION',10072,7,c,b,j,14,cv+'Version',cv+cQ+'parameter used for logging',b,False,False]),
_(['DCI_LOGGED_'+fv,10073,7,c,b,j,21,fw+fx,fw+fx,b,False,False]),
_([X+cS,10074,7,c,J+D+F,d,4,cT,cx+bA+'of client','IP',False,False]),
_([X+'PORT_NUMBER',10075,7,c,b,d,1,cX,'Port number',b,False,False],[[0,1,2,3,4,5,6,7],['Unknown Port Value','HTTP Port','HTTPS Port','MODBUS TCP Port','BACNET Port','BLE Port','Ethernet IP over TCP Port','Ethernet IP over UDP Port']]),
_([fE+fF,10076,7,c,b,p,1,db+'TCP',db+'TCP is '+dc+dg+dh+dk+dl+dm+dp,b,False,False],[[0,1],['Disable','Enable']]),
_(['DCI_BACNET_'+gj,10077,7,c,b,p,1,fH+'IP',fH+'IP is '+dc+dg+dh+dk+dl+dm+dp,b,False,False],[[0,1],['Disable','Enable']]),
_(['DCI_ENABLE_SESSION_TAKEOVER',10078,7,c,b,p,1,K+'Session Takeover','Enable/Disable the session takeover(enable = 1, disable '+bH,b,False,False]),
_([fZ+'CONCURRENT_SESSION',10079,7,c,b,d,1,bL+'Concurrent Sessions',bL+'number of sessions '+gf+'can have.',b,False,False],[1,3]),
_(['DCI_ABSOLUTE_TIMEOUT_SEC',10080,7,c,b,s,1,'User Absolute '+fS,bL+'time(sec) '+dF+fT+'a session '+bK+'terminated automatically',G,False,False],[dv,43200]),
_([bI+'LOCK_TIME_SEC',10081,7,c,b,h,1,'User Lock '+bJ,'A user '+bK+'locked for this period of time(sec) if maximum failed '+bM+'attempt is achieved',G,False,False],[du,dv]),
_([fZ+'FAILED_LOGIN_ATTEMPTS',10082,7,c,b,h,1,bL+dx+'Login Attempts','Maimum '+bM+'attempts '+gf+'can have '+gg+gg+'account lock',b,False,False],[3,v]),
_(['DCI_CORS_'+hK,10083,7,c,b,d,1,'CORS Origin '+cP,'0 - request from all '+gh+'allowed '+bt+'allow request having '+gh+'as active IP',b,False,False],[[0,1],['(*) - allows all Origin','(Active device IP) - allows Origin with device IP']]),
_([bQ+'IP_BACNET_'+gj,10084,7,c,b,p,1,Y+bj+'list BACnet '+gk,Y+bj+gq+'BACnet '+gk,b,False,False]),
_([bQ+'IP_EIP_'+B,10085,7,c,b,p,1,Y+bj+'list EIP '+C,Y+bj+gq+'EIP Enable',b,False,False]),
_([cl+'CERT_CONTROL',10086,7,c,b,d,1,t+I+bw+bF,t+I+bw+gr+br+I+bw+'Disable. If disabled '+dJ+'will NOT generate '+dF+gv+gw+bt+I+bw+'Enable. '+gA+dJ+gC+dF+gv+gw+dG+I+bw+K+'on IP change. '+gA+dJ+gC+'on boot-up if IP is changed.',b,False,False],[[0,1,2],['Certificate Generation Disable. If disabled certificate will NOT generate after factory reset.','Certificate Generation Enable. If enabled certificate will generate after factory reset.','Certificate Generation Enable on IP change. If enabled certificate will generate on boot-up if IP is changed.']]),
_([cm+'LWM2M_'+gD+B,10087,7,c,b,p,1,gE+gF+gG+C,gE+gF+gG+C,b,False,False]),
_([cm+dT+bz,10088,7,c,b,j,64,bm+gJ+gK+u,bm+gJ+gK+u,b,False,False]),
_([cm+dT+'IP_ADDR',10089,7,c,J+D+F,d,4,bm+gR,bm+gR,'IP',False,False]),
_([cm+dT+gV,10090,7,c,b,h,1,bm+gX+gY,bm+gX+gY,b,False,False]),
_(['DCI_LICENSE_INFORMATION',10091,7,g,b,j,84,gZ+hb,gZ+hb,b,False,False]),
_([f+l+'IN_1_VOLT',10092,7,c,b,h,1,Z+by,hh+R+hj+hk,b,False,False],0,[[189,2]]),
_([f+l+'IN_1_FREQ',10093,7,c,b,h,1,Z+dZ,Z+dZ,b,False,False],0,[[190,2]]),
_([f+l+dU+cR,10094,7,c,b,h,1,Z+dV+by,R+dW+dX+'1',b,False,False],0,[[191,2]]),
_([f+l+dU+hp,10095,7,c,b,h,1,Z+dV+ct,R+dW+dX+'2',b,False,False],0,[[192,2]]),
_([f+l+dU+hs,10096,7,c,b,h,1,Z+dV+cw,R+dW+dX+'3',b,False,False],0,[[193,2]]),
_([f+l+'IN_2_VOLT',10097,7,c,b,h,1,bv+bB,hh+R+hj+hk,b,False,False],0,[[194,2]]),
_([f+l+'IN_2_FREQ',10098,7,c,b,h,1,bv+cG,Z+dZ,b,False,False],0,[[195,2]]),
_([f+l+fc+cR,10099,7,c,b,h,1,bv+w+by,R+cD+w+'1',b,False,False],0,[[196,2]]),
_([f+l+fc+hp,10100,7,c,b,h,1,bv+w+ct,R+cD+w+'2',b,False,False],0,[[197,2]]),
_([f+l+fc+hs,10101,7,c,b,h,1,bv+w+cw,R+cD+w+'3',b,False,False],0,[[198,2]]),
_([f+l+'OUT_ACT_PWR',10102,7,c,b,s,1,q+hx,q+hx,b,False,False],0,[[199,4]]),
_([f+l+'OUT_APP_PWR',10103,7,c,b,s,1,q+'VA',q+'VA',b,False,False],0,[[201,4]]),
_([f+l+'OUT_CURRENT',10104,7,c,b,bh,1,q+hz,q+hz,b,False,False],0,[[203,4]]),
_([f+l+'OUT_EFF',10105,7,c,b,d,1,q+hA,q+hA,b,False,False],0,[[205,1]]),
_([f+l+'OUT_FREQ',10106,7,c,b,h,1,q+cG,q+cG,b,False,False],0,[[206,2]]),
_([f+l+'OUT_VOLT',10107,7,c,b,h,1,q+bB,q+bB,b,False,False],0,[[207,2]]),
_([f+l+cJ+'1_PER_'+cK,10108,7,c,b,d,1,q+w+by,q+w+by,b,False,False],0,[[208,1]]),
_([f+l+cJ+'2_PER_'+cK,10109,7,c,b,d,1,q+w+ct,q+w+ct,b,False,False],0,[[209,1]]),
_([f+l+cJ+'3_PER_'+cK,10110,7,c,b,d,1,q+w+cw,q+w+cw,b,False,False],0,[[hJ,1]]),
_([f+l+hK,10111,7,c,b,d,1,bp+cP,bp+cP,b,False,False],[[1,2,3,4,5],['1 IN - 1 OUT','2 IN - 2 OUT','3 IN - 1 OUT','3 IN - 3 OUT','1 IN - 3 OUT']],[[211,1]]),
_([f+L+'I_PART_NUM',10112,7,c,b,j,11,hP,hP,b,False,False],0,[[212,11]]),
_([f+L+'VOLT',10113,7,c,b,h,1,hQ+bB,hQ+bB,b,False,False],0,[[218,2]]),
_([f+bs+'BAT_POL_'+cR,10114,7,c,b,h,1,A+hT+hU,A+hT+hU,b,False,False],0,[[219,2]]),
_([f+L+'TEMP',10115,7,c,b,h,1,hV,hV,'Kelvin',False,False],0,[[220,2]]),
_([f+L+'RUNTIME_TO_EMP',10116,7,c,b,s,1,hW+hX,hW+hX,G,False,False],0,[[221,4]]),
_([f+L+'REM_CAP',10117,7,c,b,d,1,hY+hZ,hY+hZ,b,False,False],0,[[223,1]]),
_([f+L+'PER_LOAD',10118,7,c,b,d,1,jb+jc,jb+jc,b,False,False],0,[[224,1]]),
_([f+L+jd,10119,7,c,b,d,1,jf+cy,jf+cy,b,False,False],[[0,1,2,3,4,5,6,7,8,9,v,11,r,T],['Power On mode','Standby mode','Bypass mode','Line mode','Battery mode','Battery Test mode','Fault mode','Converter mode','ECO mode','Shut Down mode','Boost state','Buck state','Other','ESS mode']],[[225,1]]),
_([f+L+'ALM_CODE',10120,7,c,b,d,1,jj+cB,jj+cB,b,False,False],0,[[226,1]]),
_([f+l+'IN_2_SWOFF_CON',10121,7,c,b,d,1,bp+jl+bF,bp+jl+bF,b,False,False],[[0,1],['not used','Switch off automatic bypass']],[[227,1]]),
_([f+l+'IN_2_SWON_CON',10122,7,c,b,d,1,bp+jq+bF,bp+jq+bF,b,False,False],[[0,1],['not used','Switch on automatic bypass']],[[228,1]]),
_([f+cY+'OUTL_1_'+cZ,10123,7,c,b,d,1,bq+js,bq+js,b,False,False],[[0,1,2],['Not powered','Not protected','Protected']],[[229,1]]),
_([f+cY+'OUTL_2_'+cZ,10124,7,c,b,d,1,bq+jt,bq+jt,b,False,False],[[0,1,2],['Not powered','Not protected','Protected']],[[230,1]]),
_([f+cY+'OUTL_3_'+cZ,10125,7,c,b,d,1,bq+jw,bq+jw,b,False,False],[[0,1,2],['Not powered','Not protected','Protected']],[[231,1]]),
_([f+bs+'BAT_TEST',10126,7,c,b,d,1,A+jx,A+jx,b,False,False],[[0,1,2,3,4,5,6,7],['None','Done and Passed','Done and Warning','Done and Error','Aborted','In progress','No test iniated','Test Scheduled']],[[232,1]]),
_([f+bs+'BAT_TEST_COM',10127,7,c,b,d,1,A+dd,df+A+'Test',b,False,False],[[0,1,2,3],['No test','Quick test','Deep test','Abort test']],[[233,1]]),
_([f+bs+jz+jA+'DUR',10128,7,c,b,h,1,A+'Duration '+dd,df+A+'Test by Duration',b,False,False],0,[[234,2]]),
_([f+bs+jz+jA+'LVL',10129,7,c,b,d,1,A+'Level '+dd,df+A+'test by '+jE,b,False,False],0,[[235,1]]),
_([f+'CONN_STAT_REASON',10130,7,c,b,d,1,dj+bP+'reason',dj+bP+'reason enums',b,False,False],[[0,1,2,3,4,5,6,7,8,9,v,11,r,T,14,E,16,17],['IoT initial state (default)','Cloud connected','IoT disabled','IoT clock not correctly set','Reconnecting','Failed to open connection','Too many lost messages','Invalid connection string','Invalid configuration','Daily cloud message limit reached','SAS token expired','Device disabled by user on IoT hub','Retry expired','No network','Communication error','Unknown error','TLS initialization failure','Ethernet/network interface link down']],[[236,1]]),
_(['DCI_LANG_PREF_SETTING',10131,7,c,b,d,1,jU+cb+jV,jU+cb+jV,b,False,False],[[0,1,2,3],['Use Device stored language setting','Use Device stored language setting per user','Use browser stored language setting','Use browser local language setting']]),
_(['DCI_COMMON_LANG_PREF',10132,7,c,b,j,6,jZ+cb+'Preferrence',jZ+cb+'Preferrence according to ISO 639-1',b,False,False]),
_(['DCI_DATA_LOG',10133,7,g,b,h,1,'Data Log','Data logging for predefined '+kc+'at fixed interval, change of value or manual trigger',b,False,False]),
_(['DCI_EVENT_LOG',10134,7,g,b,h,1,dA+'Log','Application specific Events Log',b,False,False]),
_([cd+'POWER_LOG',10135,7,g,b,h,1,'Audit-Power Log',cj+ck+dB+'and reset',b,False,False]),
_([cd+'FW_UPGRADE_LOG',10136,7,g,b,h,1,'Audit-Fw-Update Log',cj+ck+'firmware update',b,False,False]),
_([cd+'USER_LOG',10137,7,g,b,h,1,'Audit-User Log',cj+ck+'user',b,False,False]),
_([cd+'CONFIG_LOG',10138,7,g,b,h,1,'Audit-Config Log',cj+ck+'configuration',b,False,False]),
_([cl+'UP_TIME_SEC',10139,7,g,b,s,1,t+'Up Time',t+'up time since last reboot(in seconds)',G,False,False]),
_(['DCI_PTP_'+B,10140,7,c,b,p,1,K+kx+'PTP',K+kx+'PTP',b,False,False],[[0,1],['PTP disable','PTP enable']]),
];
// ASM array contains object with keys: id, name, pid
// [pid] is an array of DCI pid
_ = function (a){
    return {id:a[0],name:a[1],pid:a[2]};
};
window.ASM=[
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([1,M+'1',[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([12,x+y,[0]]),
_([13,cA,[0]]),
_([13,cA,[0]]),
_([13,cA,[0]]),
_([13,cA,[0]]),
];
}})();