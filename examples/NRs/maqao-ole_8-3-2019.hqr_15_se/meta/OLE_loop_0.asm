0x402d88; INC	%RDX
0x402d8b; LEA	(%RSI,%RAX,1),%R14
0x402d8f; MOVSS	(%R14,%R9,1),%XMM1
0x402d95; SUBSS	%XMM0,%XMM1
0x402d99; MOVSS	%XMM1,(%R14,%R9,1)
0x402d9f; LEA	(%R15,%RAX,1),%R14
0x402da3; MOVSS	0x4(%R14,%R9,1),%XMM2
0x402daa; SUBSS	%XMM0,%XMM2
0x402dae; MOVSS	%XMM2,0x4(%R14,%R9,1)
0x402db5; LEA	(%R10,%RAX,1),%R14
0x402db9; MOVSS	0x8(%R14,%R9,1),%XMM3
0x402dc0; SUBSS	%XMM0,%XMM3
0x402dc4; MOVSS	%XMM3,0x8(%R14,%R9,1)
0x402dcb; LEA	(%RBP,%RAX,1),%R14
0x402dd0; MOVSS	0xc(%R14,%R9,1),%XMM4
0x402dd7; SUBSS	%XMM0,%XMM4
0x402ddb; MOVSS	%XMM4,0xc(%R14,%R9,1)
0x402de2; LEA	(%RBX,%RAX,1),%R14
0x402de6; MOVSS	0x10(%R14,%R9,1),%XMM5
0x402ded; SUBSS	%XMM0,%XMM5
0x402df1; MOVSS	%XMM5,0x10(%R14,%R9,1)
0x402df8; LEA	(%RDI,%RAX,1),%R14
0x402dfc; MOVSS	0x14(%R14,%R9,1),%XMM6
0x402e03; SUBSS	%XMM0,%XMM6
0x402e07; MOVSS	%XMM6,0x14(%R14,%R9,1)
0x402e0e; LEA	(%R13,%RAX,1),%R14
0x402e13; MOVSS	0x18(%R14,%R9,1),%XMM7
0x402e1a; SUBSS	%XMM0,%XMM7
0x402e1e; MOVSS	%XMM7,0x18(%R14,%R9,1)
0x402e25; LEA	(%R11,%RAX,1),%R14
0x402e29; MOVSS	0x1c(%R14,%R9,1),%XMM8
0x402e30; ADD	$0x20,%RAX
0x402e34; SUBSS	%XMM0,%XMM8
0x402e39; MOVSS	%XMM8,0x1c(%R14,%R9,1)
0x402e40; ADD	%R8,%R9
0x402e43; CMP	%R12,%RDX
0x402e46; JB	402d88 <codelet_+0x88>
