#pragma once

#define MAX_FRAME_COUNT 32

class CStackTrace
{
	public:
		CStackTrace();
		~CStackTrace() = default;

	public:
		std::vector<std::string> get_stack_trace();
		void print_stack_trace();

	protected:
		int capture_stack_trace(int frames_to_skip, int max_frames, void **out_frames);
		std::vector <std::string> get_stack_frames_text(void **frames, int num_frames);

	private:
		int num_frames;
		mutable void *frames[MAX_FRAME_COUNT];
		mutable std::string buffer;
};