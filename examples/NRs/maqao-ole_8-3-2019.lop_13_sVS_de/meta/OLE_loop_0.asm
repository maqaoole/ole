0x402d80; MOVSD	0x10(%R11,%R15,8),%XMM2
0x402d87; MOVAPS	%XMM2,%XMM4
0x402d8a; MULSD	%XMM2,%XMM4
0x402d8e; MULSD	%XMM0,%XMM2
0x402d92; MOVSD	0x10(%RBP,%R15,8),%XMM3
0x402d99; SUBSD	%XMM2,%XMM3
0x402d9d; ADDSD	0x10(%RBX,%R15,8),%XMM3
0x402da4; ADDSD	0x18(%R11,%R15,8),%XMM3
0x402dab; ADDSD	0x8(%R11,%R15,8),%XMM3
0x402db2; MULSD	%XMM1,%XMM3
0x402db6; ADDSD	%XMM3,%XMM4
0x402dba; MOVSD	%XMM4,0x10(%R14,%R15,8)
0x402dc1; INC	%R15
0x402dc4; CMP	%R10,%R15
0x402dc7; JB	402d80 <codelet_+0x80>