# class project on EECS 322 - complier construction.

The whole system compile LB language to assembly language. The compiling process is breaking down to 6 parts. Each part will take care of partial of the problem.

1. L1 -> Assembly: L1 is a fairly easy language and is already pretty close to Assembly. Served as bridging between L2 and assembly.
2. L2 -> L1: L2 introduce variable, considering L1 and assembly contains only registers, L2 -> L1 will solve the register allocation problem, accomplished following graph coloring method.
3. L3 -> L2: L3 introduced highly generalized grammar and is closer to higher level language. L3 -> L2 will map those generalized instructions into easy (but usually less representative) L2 instructions. This is done by tiling system.
4. IR -> L3: IR introduced type system, control flow by basic blocks and allow for easier grammar for array usage. IR -> L3 will translate array access to memory access. High dimensional array will linearized into sequential memory allocation.
5. LA -> IR: This step will encode values and check array access, also generating basic blocks used in IR.
6. LB -> LA: LB introduced nested scope and control structure (if & while loop). LB -> LA will linearize the scopes and represent the control structure properly.

There is a contest after the quarter to see whose complier can generate more efficient code, my code won. Some useful optimization are as follow:

1. LA -> IR: when array access is checked in one function, the length of dimension is stored, this will save memory access in the next time the length is used.
2. LA -> IR: if an Array allocation is checked, do not check it again later. (may introduce bug though, "later" should means later in the same control flow, but my code didn't actually take care of it.)
3. L2 -> L1: use callee save registers on register allocation. (This is a bug fix, as callee saved registers are not used)
4. L3 -> L2: MISC (rdi @ rdi rsi 4) are faster than other approach when it could be used, use it.
5. L2 -> L1: remove un-necessary instruction (e.g: (r10 <- r10)).
6. L2 -> L1: remove un-necessary memory access (e.g: ((mem rsp 8) <- r10) \n (r10 <- (mem rsp 8)))
