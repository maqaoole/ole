0x402d8d; MOVSD	(%RAX,%R10,1),%XMM9
0x402d93; INC	%RCX
0x402d96; DIVSD	%XMM8,%XMM9
0x402d9b; MOVSD	%XMM9,(%RAX,%R10,1)
0x402da1; MULSD	%XMM9,%XMM9
0x402da6; MOVSD	0x8(%RAX,%R10,1),%XMM10
0x402dad; ADDSD	%XMM9,%XMM7
0x402db2; DIVSD	%XMM8,%XMM10
0x402db7; MOVSD	0x10(%RAX,%R10,1),%XMM11
0x402dbe; MOVSD	0x18(%RAX,%R10,1),%XMM12
0x402dc5; MOVSD	0x20(%RAX,%R10,1),%XMM13
0x402dcc; MOVSD	0x28(%RAX,%R10,1),%XMM14
0x402dd3; MOVSD	0x30(%RAX,%R10,1),%XMM15
0x402dda; MOVSD	0x38(%RAX,%R10,1),%XMM9
0x402de1; DIVSD	%XMM8,%XMM11
0x402de6; DIVSD	%XMM8,%XMM12
0x402deb; DIVSD	%XMM8,%XMM13
0x402df0; DIVSD	%XMM8,%XMM14
0x402df5; DIVSD	%XMM8,%XMM15
0x402dfa; DIVSD	%XMM8,%XMM9
0x402dff; MOVSD	%XMM10,0x8(%RAX,%R10,1)
0x402e06; MULSD	%XMM10,%XMM10
0x402e0b; MOVSD	%XMM11,0x10(%RAX,%R10,1)
0x402e12; ADDSD	%XMM10,%XMM5
0x402e17; MULSD	%XMM11,%XMM11
0x402e1c; MOVSD	%XMM12,0x18(%RAX,%R10,1)
0x402e23; ADDSD	%XMM11,%XMM4
0x402e28; MULSD	%XMM12,%XMM12
0x402e2d; MOVSD	%XMM13,0x20(%RAX,%R10,1)
0x402e34; ADDSD	%XMM12,%XMM3
0x402e39; MULSD	%XMM13,%XMM13
0x402e3e; MOVSD	%XMM14,0x28(%RAX,%R10,1)
0x402e45; ADDSD	%XMM13,%XMM2
0x402e4a; MULSD	%XMM14,%XMM14
0x402e4f; MOVSD	%XMM15,0x30(%RAX,%R10,1)
0x402e56; ADDSD	%XMM14,%XMM1
0x402e5b; MULSD	%XMM15,%XMM15
0x402e60; MOVSD	%XMM9,0x38(%RAX,%R10,1)
0x402e67; ADD	$0x40,%RAX
0x402e6b; MULSD	%XMM9,%XMM9
0x402e70; ADDSD	%XMM15,%XMM6
0x402e75; ADDSD	%XMM9,%XMM0
0x402e7a; CMP	%R13,%RCX
0x402e7d; JB	402d8d <codelet_+0x8d>