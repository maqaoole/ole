0x400cb9; MOVUPS	(%RCX,%RSI,4),%XMM1
0x400cbd; MOVAPS	%XMM1,%XMM3
0x400cc0; MULPS	%XMM1,%XMM3
0x400cc3; MOVUPS	0x4(%R10,%RSI,4),%XMM2
0x400cc9; MOVUPS	(%RDX,%RSI,4),%XMM0
0x400ccd; ADDPS	%XMM0,%XMM3
0x400cd0; MULPS	%XMM0,%XMM0
0x400cd3; ADDPS	%XMM0,%XMM3
0x400cd6; ADDPS	%XMM1,%XMM3
0x400cd9; MOVUPS	%XMM3,(%R10,%RSI,4)
0x400cde; ADDPS	%XMM3,%XMM2
0x400ce1; MOVUPS	%XMM2,(%R8,%RSI,4)
0x400ce6; ADD	$0x4,%RSI
0x400cea; CMP	%RBP,%RSI
0x400ced; JB	400cb9 <codelet_+0xf9>