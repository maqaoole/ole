0x402d6b; MOVSD	(%R10,%RSI,1),%XMM1
0x402d71; INC	%R9
0x402d74; MOVSD	0x8(%R10,%RSI,1),%XMM2
0x402d7b; MOVSD	0x10(%R10,%RSI,1),%XMM3
0x402d82; MOVSD	0x18(%R10,%RSI,1),%XMM4
0x402d89; MOVSD	0x20(%R10,%RSI,1),%XMM5
0x402d90; MOVSD	0x28(%R10,%RSI,1),%XMM6
0x402d97; MOVSD	0x30(%R10,%RSI,1),%XMM7
0x402d9e; MOVSD	0x38(%R10,%RSI,1),%XMM8
0x402da5; DIVSD	%XMM0,%XMM1
0x402da9; DIVSD	%XMM0,%XMM2
0x402dad; DIVSD	%XMM0,%XMM3
0x402db1; DIVSD	%XMM0,%XMM4
0x402db5; DIVSD	%XMM0,%XMM5
0x402db9; DIVSD	%XMM0,%XMM6
0x402dbd; DIVSD	%XMM0,%XMM7
0x402dc1; DIVSD	%XMM0,%XMM8
0x402dc6; MOVSD	%XMM1,(%R10,%RAX,1)
0x402dcc; MOVSD	%XMM2,0x8(%R10,%RAX,1)
0x402dd3; MOVSD	%XMM3,0x10(%R10,%RAX,1)
0x402dda; MOVSD	%XMM4,0x18(%R10,%RAX,1)
0x402de1; MOVSD	%XMM5,0x20(%R10,%RAX,1)
0x402de8; MOVSD	%XMM6,0x28(%R10,%RAX,1)
0x402def; MOVSD	%XMM7,0x30(%R10,%RAX,1)
0x402df6; MOVSD	%XMM8,0x38(%R10,%RAX,1)
0x402dfd; ADD	$0x40,%R10
0x402e01; CMP	%RCX,%R9
0x402e04; JB	402d6b <codelet_+0x6b>