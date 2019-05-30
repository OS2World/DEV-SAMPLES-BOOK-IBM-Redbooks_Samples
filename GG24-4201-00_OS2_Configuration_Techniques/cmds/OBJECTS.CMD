/* */

call rxfuncadd sysloadfuncs, rexxutil, sysloadfuncs
call sysloadfuncs

call SysQueryClassList "list."
do i = 1 to list.0
   say 'Class' i 'is' list.i
end

Call SysDropFuncs
