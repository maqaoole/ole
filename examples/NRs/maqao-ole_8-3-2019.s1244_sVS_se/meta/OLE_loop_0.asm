0x400bd2; MOVSS	(%RCX,%RSI,4),%XMM3
0x400bd7; MOVAPS	%XMM3,%XMM1
0x400bda; MULSS	%XMM3,%XMM1
0x400bde; MOVSS	(%RDX,%RSI,4),%XMM0
0x400be3; MOVSS	0x4(%R9,%RSI,4),%XMM2
0x400bea; ADDSS	%XMM0,%XMM1
0x400bee; MULSS	%XMM0,%XMM0
0x400bf2; ADDSS	%XMM0,%XMM1
0x400bf6; ADDSS	%XMM1,%XMM3
0x400bfa; MOVSS	%XMM3,(%R9,%RSI,4)
0x400c00; ADDSS	%XMM3,%XMM2
0x400c04; MOVSS	%XMM2,(%R8,%RSI,4)
0x400c0a; INC	%RSI
0x400c0d; CMP	%RDI,%RSI
0x400c10; JL	400bd2 <codelet_+0x12>