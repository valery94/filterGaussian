__kernel void Gaus_3D(__global unsigned char* imageIn,
	__global unsigned char* imageOut,
	__constant float* filter,
	int rows,
	int cols,
	int filterWidth,
	__local unsigned char* localImage,
	int localHeight,
	int localWidth,
	int bufferCounter)
{
	// Determine the amount of padding for this filter
	int filterRadius = (filterWidth / 2);
	int padding = filterRadius * 2;

	// Determine the size of the workgroup output region
	int groupStartCol = get_group_id(0)*get_local_size(0);
	int groupStartRow = get_group_id(1)*get_local_size(1);

	// Determine the local ID of each work-item
	int localCol = get_local_id(0);
	int localRow = get_local_id(1);

	// Determine the global ID of each work-item. work-items
	// representing the output region will have a unique global
	// ID
	int globalCol = groupStartCol + localCol;
	int globalRow = groupStartRow + localRow;

	// Cache the data to local memory
	// Step down rows
	int icounter = 0;
	int startFrameNumber = bufferCounter;
	for (int x = -filterRadius; x <= filterRadius; x++, icounter++)
	{
		for (int i = localRow; i < localHeight; i += get_local_size(1))
		{
			int curRow = groupStartRow + i;
			// Step across columns
			for (int j = localCol; j < localWidth; j += get_local_size(0))
			{
				int curCol = groupStartCol + j;
				// Perform the read if it is in bounds
				if (curRow < rows && curCol < cols)
				{
					if (startFrameNumber >= filterWidth)
					{
						startFrameNumber = 0;
					}
					localImage[icounter*(localHeight)*(localWidth)+j + i*localWidth] = imageIn[(icounter*rows*cols) + curRow*cols + curCol];
				}
			}
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	startFrameNumber = bufferCounter+1;
	// Perform the convolution
	if (globalRow < rows - padding && globalCol < cols - padding)
	{
		// Each work-item will filter around its start location
		//(starting from the filter radius left and up)
		float sum = 0.0f;
		int filterIdx = 0;
		// Not unrolled
		for (int x = -filterRadius, icounter = 0; x <= filterRadius; x++, startFrameNumber++, icounter++)
		{
			for (int i = localRow; i < localRow + filterWidth; i++)
			{
				int offset = i*localWidth;
				for (int j = localCol; j < localCol + filterWidth; j++)
				{
					if (startFrameNumber == filterWidth)
					{
						startFrameNumber = 0;
					}
					//sum += localImage[icounter * (localHeight)*(localWidth)+offset + j] * filter[filterIdx++];
					sum += localImage[startFrameNumber * (localHeight)*(localWidth)+offset + j] * filter[filterIdx++];
				}
			}
		}
		// Write the data out
		imageOut[(globalRow + filterRadius)*cols + (globalCol + filterRadius)] = sum;
	}
	return;
}