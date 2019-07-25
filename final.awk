# Attention! You may need to run 'rm -f final' if you run 'gawk ...>>'  
BEGIN { 
	FS="[() \t]";
	packets=100;#the packets each node will send, you need to change it.
	posnum=32 #the amount of nodes,include sink node, you need to change it.
}
{
	action = $1;
	time = $2;
	for (i=1;i<=NF;i++){
		if (action=="d"||action=="t"||action=="-")   break;
		if ($i ~ /Sequence/)   packet_id = $(i+3)+0;
		if ( ($i ~ /m_addrShortDstAddr/) && (!match($(i+2),"00:01")) )   break;
		if ($i ~ /m_addrShortSrcAddr/){
			SrcAddr = $(i+2);
			if ( match($(i+10),"ns3::Icmpv6NS") || match($(i+10),"ns3::Icmpv6NA") )        break;
			str1="2"; str2="0"; str3=str2 str1;
			for(j=0; j<posnum-1; j++){
				m_SrcAddr="00:"str3;
				if(match(SrcAddr, m_SrcAddr)){     
					if (action=="+"){
						++packs[j]; start_time1[j,packs[j]]=time;  start_timeid[j,packs[j]]=packet_id; }
					if (action=="r"&&end_timeid[j,packt[j]]!=packet_id){
						++packt[j]; end_time[j,packt[j]]=time;	end_timeid[j,packt[j]]=packet_id; } }
				if(str1=="0"||str1=="2"||str1=="3"||str1=="4")    str1=str1+1;
				else if(str1=="5"||str1=="6"||str1=="7"||str1=="8")  str1=str1+1;
				else if(str1=="9")  str1="a";		else if(str1=="a")  str1="b";		
				else if(str1=="b")  str1="c";		else if(str1=="c")  str1="d";		
				else if(str1=="d")  str1="e";		else if(str1=="e")  str1="f";
				else if(str2=="0"||str2=="2"||str2=="3"||str2=="4") {str2=str2+1;str1="0";}
				else if(str2=="5"||str2=="6"||str2=="7"||str2=="8") {str2=str2+1;str1="0";}
				else if(str2=="9")  {str2="a";str1="0";}		else if(str2=="a")  {str2="b";str1="0";}		
				else if(str2=="b")  {str2="c";str1="0";}		else if(str2=="c")  {str2="d";str1="0";}
				else if(str2=="d")  {str2="e";str1="0";}		else if(str2=="e")  {str2="f";str1="0";}
				str3=str2 str1; } } }
}
END {
	printf("node1: the first node is the mobile sink node.\n");
	for( j=0; j<posnum-1; j++){
		num2=1;
		for ( num1=1; num1 <= packets; num2++){
			if(start_timeid[j,num2]==end_timeid[j,num1] && ((end_time[j,num1]-start_time1[j,num2])<1)  ) {
				start_time[j,num1]=start_time1[j,num2];
				num1++;   } } }
	sum1=0;sum2=0;totaltime=0.0;totalpackets=0;
	for( i=0; i<posnum-1; i++){
		for ( num1=1; num1 <= packets; num1++){
			start=start_time[i,num1];
			end=end_time[i,num1];
			packet_duration=end-start;
			if ( start < end ) {
				printf("node%d: %d  send:%f  receive:%f  %f\n",i+2, num1, start, end, sum1=sum1+packet_duration);
				sum2+=end;
				num=num1;
				if(end>totaltime) totaltime=end; } }
		totalpackets+=num;
		printf("---------------------------------------------------------\n");
	}
	printf("total collection time is %f\n",totaltime);
	printf("time average delay is %f\n",sum1/totalpackets);
	printf("collection average delay is %f\n",sum2/totalpackets);
	printf("real collection total packets is %d\n",totalpackets);
	printf("ideal collection total packets is %d\n",packets*(posnum-1));
printf("collection packets percentage is %.2f%\n",(totalpackets+0.0)*100/(packets*(posnum-1)));}
