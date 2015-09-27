
__kernel void multi(
		__global float* a,
		const unsigned int count)
{
	int i = get_global_id(0);
	if(i < count)
	a[i] = a[i] * 2;
}
