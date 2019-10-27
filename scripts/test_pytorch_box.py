import time
import torch

input = torch.ones(3, 1536, 2560)
def process(input):
	input = input.unsqueeze(0)
	kernel = torch.ones(3, 1, 1, 3)
	blur_x = torch.nn.functional.conv2d(input, kernel, groups=3)
	kernel = kernel.permute(0, 1, 3, 2)
	blur_y = torch.nn.functional.conv2d(blur_x, kernel, groups=3)
	return blur_y

num_trials = 20
start = time.time()
for i in range(num_trials):
	process(input)
end = time.time()
print((end - start) / num_trials)