open "counter" for input as #1

input #1,value

close #1

value=value+1

if value >=8 then value = 1:open "dl.log" for output as #1:close #1:open "ul.log" for output as #1:close #1

open "counter" for output as #1
print #1,value
close #1



