function summator(param,input,cust) 
	send(cust,input+param)
end
function umnog(cust,param,input)  
	if param==nil then
		become("umnog",cust,input)
	else
		send(cust,param*input)
	end
end
--Первоначальное создание и инициализация
function createAndInitActors()
	print_act=create("final_print")
	umnog_act=create("umnog",print_act,nil)
	summator1_act=create("summator",2)
	summator2_act=create("summator",3)
	init(summator1_act,summator2_act)
end

function init(summator1_act,summator2_act)
	send(summator1_act,10,umnog_act);
	send(summator2_act,11,umnog_act);
end