0x402d30; MOVSD	(%RCX,%RSI,1),%XMM0
0x402d35; INC	%RDI
0x402d38; MOVSD	0x8(%RCX,%RSI,1),%XMM1
0x402d3e; MOVSD	0x10(%RCX,%RSI,1),%XMM2
0x402d44; MOVSD	0x18(%RCX,%RSI,1),%XMM3
0x402d4a; MOVSD	0x20(%RCX,%RSI,1),%XMM4
0x402d50; MOVSD	0x28(%RCX,%RSI,1),%XMM5
0x402d56; MOVSD	0x30(%RCX,%RSI,1),%XMM6
0x402d5c; MOVSD	0x38(%RCX,%RSI,1),%XMM7
0x402d62; SUBSD	(%RCX,%RDX,1),%XMM0
0x402d67; SUBSD	0x8(%RCX,%RDX,1),%XMM1
0x402d6d; SUBSD	0x10(%RCX,%RDX,1),%XMM2
0x402d73; SUBSD	0x18(%RCX,%RDX,1),%XMM3
0x402d79; SUBSD	0x20(%RCX,%RDX,1),%XMM4
0x402d7f; SUBSD	0x28(%RCX,%RDX,1),%XMM5
0x402d85; SUBSD	0x30(%RCX,%RDX,1),%XMM6
0x402d8b; SUBSD	0x38(%RCX,%RDX,1),%XMM7
0x402d91; MOVSD	%XMM0,(%RCX,%RSI,1)
0x402d96; MOVSD	%XMM1,0x8(%RCX,%RSI,1)
0x402d9c; MOVSD	%XMM2,0x10(%RCX,%RSI,1)
0x402da2; MOVSD	%XMM3,0x18(%RCX,%RSI,1)
0x402da8; MOVSD	%XMM4,0x20(%RCX,%RSI,1)
0x402dae; MOVSD	%XMM5,0x28(%RCX,%RSI,1)
0x402db4; MOVSD	%XMM6,0x30(%RCX,%RSI,1)
0x402dba; MOVSD	%XMM7,0x38(%RCX,%RSI,1)
0x402dc0; ADD	$0x40,%RCX
0x402dc4; CMP	%RAX,%RDI
0x402dc7; JB	402d30 <codelet_+0x30>
