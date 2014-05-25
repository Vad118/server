function summator(param,input,cust) 
	send(cust,input+param)
	sleep(1);
end
function umnog(cust,param,input)  
	if param==nil then
		become("umnog",cust,input)
	else
		send(cust,param*input)
	end
	sleep(1);
end
--Первоначальное создание и инициализация
function createAndInitActors()
	print_act=create("final_print")
	sleep(1)
	umnog_act=create("umnog",print_act,nil)
	sleep(1)
	summator1_act=create("summator",2)
	sleep(1)
	summator2_act=create("summator",3)
	sleep(1)
	init(summator1_act,summator2_act)
end

function init(summator1_act,summator2_act)
	send(summator1_act,10,umnog_act);
	sleep(1);
	send(summator2_act,11,umnog_act);
	sleep(1);
end