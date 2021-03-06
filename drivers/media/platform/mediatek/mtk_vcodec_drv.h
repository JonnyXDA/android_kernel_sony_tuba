/*
* Copyright (c) 2015 MediaTek Inc.
* Author: PC Chen <pc.chen@mediatek.com>
*         Tiffany Lin <tiffany.lin@mediatek.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef _MEDIA_PLATFORM_MEDIATEK_MTK_VCODEC_DRV_H_
#define _MEDIA_PLATFORM_MEDIATEK_MTK_VCODEC_DRV_H_

#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>

#include "vdec_drv_if.h"
#include "venc_drv_if.h"

#define MTK_VCODEC_MAX_INSTANCES	32
#define MTK_VCODEC_MAX_FRAME_SIZE	0x800000
#define MTK_VIDEO_MAX_FRAME		32
#define MTK_MAX_CTRLS			10
#define MTK_VCODEC_MAX_EXTRA_DPB	5

#define MTK_VCODEC_DRV_NAME			"mtk_vcodec_drv"
#define MTK_VCODEC_DEC_NAME			"mt81xx-vcodec-dec"
#define MTK_VCODEC_ENC_NAME			"mt81xx-vcodec-enc"

#define MTK_VENC_IRQ_STATUS_SPS          0x1
#define MTK_VENC_IRQ_STATUS_PPS          0x2
#define MTK_VENC_IRQ_STATUS_FRM          0x4
#define MTK_VENC_IRQ_STATUS_DRAM         0x8
#define MTK_VENC_IRQ_STATUS_PAUSE        0x10
#define MTK_VENC_IRQ_STATUS_SWITCH       0x20

#define MTK_VENC_IRQ_STATUS_OFFSET       0x05C
#define MTK_VENC_IRQ_ACK_OFFSET          0x060

#define MTK_VCODEC_MAX_PLANES		3

#define VDEC_HW_ACTIVE	0x10
#define VDEC_IRQ_CFG    0x11
#define VDEC_IRQ_CLR    0x10

#define VDEC_IRQ_CFG_REG	0xa4
#define NUM_MAX_ALLOC_CTX  4
#define MTK_V4L2_BENCHMARK 1
#define USE_ENCODE_THREAD  1

/**
 * enum mtk_hw_reg_idx - MTK hw register base index
 */
enum mtk_hw_reg_idx {
	VDEC_SYS,
	VDEC_MISC,
	VDEC_LD,
	VDEC_TOP,
	VDEC_CM,
	VDEC_AD,
	VDEC_AV,
	VDEC_PP,
	VDEC_HWD,
	VDEC_HWQ,
	VDEC_HWB,
	VDEC_HWG,
	NUM_MAX_VDEC_REG_BASE,
	VENC_SYS = NUM_MAX_VDEC_REG_BASE,
	VENC_LT_SYS,
	NUM_MAX_VCODEC_REG_BASE
};

/**
 * enum mtk_instance_type - The type of an MTK Vcodec instance.
 */
enum mtk_instance_type {
	MTK_INST_DECODER		= 0,
	MTK_INST_ENCODER		= 1,
};

/**
 * enum mtk_instance_state - The state of an MTK Vcodec instance.
 * @MTK_STATE_FREE - default state when instance create
 * @MTK_STATE_CREATE - vdec instance is create
 * @MTK_STATE_INIT - vdec instance is init
 * @MTK_STATE_CONFIG - reserved for encoder
 * @MTK_STATE_HEADER - vdec had sps/pps header parsed
 * @MTK_STATE_RUNNING - vdec is decoding
 * @MTK_STATE_FLUSH - vdec is flushing
 * @MTK_STATE_RES_CHANGE - vdec detect resolution change
 * @MTK_STATE_FINISH - ctx instance is stopped streaming
 * @MTK_STATE_DEINIT - before release ctx instance
 * @MTK_STATE_ERROR - vdec has something wrong
 * @MTK_STATE_ABORT - abort work in decode working thread
 */
enum mtk_instance_state {
	MTK_STATE_FREE		= 0,
	MTK_STATE_CREATE	= (1 << 0),
	MTK_STATE_INIT		= (1 << 1),
	MTK_STATE_CONFIG	= (1 << 2),
	MTK_STATE_HEADER	= (1 << 3),
	MTK_STATE_RUNNING	= (1 << 4),
	MTK_STATE_FLUSH		= (1 << 5),
	MTK_STATE_RES_CHANGE	= (1 << 6),
	MTK_STATE_FINISH	= (1 << 7),
	MTK_STATE_DEINIT	= (1 << 8),
	MTK_STATE_ERROR		= (1 << 9),
	MTK_STATE_ABORT		= (1 << 10),
};

/**
 * struct mtk_param_change - General encoding parameters type
 */
enum mtk_encode_param {
	MTK_ENCODE_PARAM_NONE = 0,
	MTK_ENCODE_PARAM_BITRATE = (1 << 0),
	MTK_ENCODE_PARAM_FRAMERATE = (1 << 1),
	MTK_ENCODE_PARAM_INTRA_PERIOD = (1 << 2),
	MTK_ENCODE_PARAM_FRAME_TYPE = (1 << 3),
	MTK_ENCODE_PARAM_SKIP_FRAME = (1 << 4),
};

/**
 * enum mtk_fmt_type - Type of the pixelformat
 * @MTK_FMT_FRAME - mtk vcodec raw frame
 */
enum mtk_fmt_type {
	MTK_FMT_DEC		= 0,
	MTK_FMT_ENC		= 1,
	MTK_FMT_FRAME		= 2,
};

/**
 * struct mtk_video_fmt - Structure used to store information about pixelformats
 */
struct mtk_video_fmt {
	char *name;
	u32 fourcc;
	enum mtk_fmt_type type;
	u32 num_planes;
};

/**
 * struct mtk_codec_framesizes - Structure used to store information about framesizes
 */
struct mtk_codec_framesizes {
	u32 fourcc;
	struct	v4l2_frmsize_stepwise	stepwise;
};

/**
 * struct mtk_q_type - Type of queue
 */
enum mtk_q_type {
	MTK_Q_DATA_SRC		= 0,
	MTK_Q_DATA_DST		= 1,
};

/**
 * struct mtk_q_data - Structure used to store information about queue
 * @colorspace	reserved for encoder
 * @field		reserved for encoder
 */
struct mtk_q_data {
	unsigned int		width;
	unsigned int		height;
	enum v4l2_field		field;
	enum v4l2_colorspace	colorspace;
	unsigned int		bytesperline[MTK_VCODEC_MAX_PLANES];
	unsigned int		sizeimage[MTK_VCODEC_MAX_PLANES];
	struct mtk_video_fmt	*fmt;
};

/**
 * struct mtk_enc_params - General encoding parameters
 * @bitrate - target bitrate
 * @num_b_frame - number of b frames between p-frame
 * @rc_frame - frame based rate control
 * @rc_mb - macroblock based rate control
 * @seq_hdr_mode - H.264 sequence header is encoded separately or joined with the first frame
 * @gop_size - group of picture size, it's used as the intra frame period
 * @framerate_num - frame rate numerator
 * @framerate_denom - frame rate denominator
 * @h264_max_qp - Max value for H.264 quantization parameter
 * @h264_profile - V4L2 defined H.264 profile
 * @h264_level - V4L2 defined H.264 level
 * @force_intra - force/insert intra frame
 * @skip_frame - encode in skip frame mode that use minimum number of bits
 */
struct mtk_enc_params {
	unsigned int	bitrate;
	unsigned int	num_b_frame;
	unsigned int	rc_frame;
	unsigned int	rc_mb;
	unsigned int	seq_hdr_mode;
	unsigned int	gop_size;
	unsigned int	framerate_num;
	unsigned int	framerate_denom;
	unsigned int	h264_max_qp;
	unsigned int	h264_profile;
	unsigned int	h264_level;
	unsigned int	force_intra;
	unsigned int	skip_frame;
};

/**
 * struct mtk_vcodec_pm - Power management data structure
 */
struct mtk_vcodec_pm {
	struct clk	*syspll_d3;
	struct clk	*vdecpll;
	struct clk	*vdec_sel;
	struct device	*larbvdec;
	struct device	*larbvenc;
	struct device	*larbvenclt;
	struct device	*pmvenc;
	struct device	*pmvenclt;
	struct device	*dev;
	struct mtk_vcodec_dev *mtkdev;
};

/**
 * struct mtk_video_buf - Private data embeded with VB2 buffer.
 * @b:			VB2 buffer
 * @list:			link list
 * @used:		Output buffer contain decoded frame data
 * @ready_to_display:	Output buffer not display yet
 * @nonrealdisplay:	Output buffer is not display frame
 * @queued_in_vb2:	Output buffer is queue in vb2
 * @queued_in_v4l2:	Output buffer is in v4l2
 * @lastframe:		Intput buffer is last buffer - EOS
 * @frame_buffer:		Decode status of output buffer
 * @lock:			V4L2 and decode thread should get mutex
 *			before r/w info in mtk_video_buf
 */
struct mtk_video_buf {
	struct vb2_buffer b;
	struct list_head list;

	bool used;
	bool ready_to_display;
	bool nonrealdisplay;
	bool queued_in_vb2;
	bool queued_in_v4l2;
	bool lastframe;
	struct vdec_fb frame_buffer;
	struct mutex lock;
};

/**
 * struct mtk_video_enc_buf - Private data related to each VB2 buffer.
 * @b:			Pointer to related VB2 buffer.
 * @param_change:	Types of encode parameter change before encode this
 *			buffer
 * @enc_params		Encode parameters changed before encode this buffer
 */
struct mtk_video_enc_buf {
	struct vb2_buffer b;
	struct list_head list;

	enum mtk_encode_param param_change;
	struct mtk_enc_params enc_params;
};

/**
 * struct mtk_vcodec_ctx - Context (instance) private data.
 *
 * @type:		type of the instance - decoder or encoder
 * @dev:		pointer to the mtk_vcodec_dev of the device
 * @fh:			struct v4l2_fh
 * @m2m_ctx:		pointer to the v4l2_m2m_ctx of the context
 * @q_data:		store information of input and output queue
 *			of the context
 * @idx:		index of the context that this structure describes
 * @state:		state of the context
 * @aborting:
 * @param_change:
 * @param_change_mutex:
 * @enc_params:		encoding parameters
 * @colorspace:
 *
 * @h_dec:		decoder handler
 * @h_enc:		encoder handler
 * @seq:		store width/height of image and buffer for decoder
 *			and encoder
 * @pb_count:		count of the DPB buffers required by MTK Vcodec hw
 * @hdr:
 *
 * @int_cond:		variable used by the waitqueue
 * @int_type:		type of the last interrupt
 * @queue:		waitqueue that can be used to wait for this context to
 *			finish
 * @irq_status:
 *
 * @display_time_info:	display time for destination buffers
 * @in_time_idx:	latest insert display time index
 * @out_time_idx:	first remove display time index
 *
 * @ctrl_hdl:		handler for v4l2 framework
 * @ctrls:		array of controls, used when adding controls to the
 *			v4l2 control framework
 *
 * @decode_work:	worker for the decoding
 * @encode_work:	worker for the encoding
 * @last_decoded_picinfo:	pic information get from latest decode
 */
struct mtk_vcodec_ctx {
	enum mtk_instance_type type;
	struct mtk_vcodec_dev *dev;
	struct v4l2_fh fh;
	struct v4l2_m2m_ctx *m2m_ctx;
	struct mtk_q_data q_data[2];
	int idx;
	enum mtk_instance_state state;
	int aborting;
	enum mtk_encode_param param_change;
	struct mutex encode_param_mutex;
	struct mutex vb2_mutex;
	struct mtk_enc_params enc_params;

	unsigned long h_dec;
	unsigned long h_enc;
	struct vdec_pic_info picinfo;
	int dpb_count;
	int hdr;

	int int_cond;
	int int_type;
	wait_queue_head_t queue;
	unsigned int irq_status;

	struct v4l2_ctrl_handler ctrl_hdl;
	struct v4l2_ctrl *ctrls[MTK_MAX_CTRLS];

	struct work_struct decode_work;
	struct work_struct encode_work;
	struct vdec_pic_info last_decoded_picinfo;

#if MTK_V4L2_BENCHMARK
	unsigned int total_enc_dec_cnt;
	unsigned int total_enc_dec_time;
	unsigned int total_enc_hdr_time;
	unsigned int total_enc_dec_init_time;

	unsigned int total_qbuf_out_time;
	unsigned int total_qbuf_cap_time;
	unsigned int total_qbuf_out_cnt;
	unsigned int total_qbuf_cap_cnt;
	unsigned int total_dqbuf_out_time;
	unsigned int total_dqbuf_cap_time;
	unsigned int total_dqbuf_out_cnt;
	unsigned int total_dqbuf_cap_cnt;
	unsigned int total_dqbuf_cnt;
	unsigned int total_expbuf_time;
#endif

};

/**
 * struct mtk_vcodec_dev - driver data
 * @v4l2_dev:		V4L2 device to register video devices for.
 * @vfd_dec:		Video device for decoder.
 * @vfd_enc:		Video device for encoder.
 *
 * @m2m_dev_dec:	m2m device for decoder.
 * @m2m_dev_enc:	m2m device for encoder.
 * @plat_dev:		platform device
 * @alloc_ctx:		VB2 allocator context
 *			(for allocations without kernel mapping).
 * @ctx:		array of driver contexts
 *
 * @curr_ctx:	The context that is waiting for codec hardware
 *
 * @reg_base:		Mapped address of MTK Vcodec registers.
 *
 * @instance_mask:	used to mark which contexts are opened
 * @num_instances:	counter of active MTK Vcodec instances
 *
 * @decode_workqueue:	decode work queue
 * @encode_workqueue:	encode work queue
 *
 * @int_cond:		used to identify interrupt condition happen
 * @int_type:		used to identify what kind of interrupt condition happen
 * @dev_mutex:		video_device lock
 * @queue:		waitqueue for waiting for completion of device commands
 *
 * @dec_irq:		decoder irq resource.
 * @enc_irq:		encoder irq resource.
 * @enc_lt_irq:		encoder lt irq resource.
 *
 * @dec_mutex:		decoder hardware lock.
 * @enc_mutex:		encoder hardware lock.
 *
 * @pm:			power management control
 * @dec_conv:		color converter device for decoding
 */
struct mtk_vcodec_dev {
	struct v4l2_device	v4l2_dev;
	struct video_device	*vfd_dec;
	struct video_device	*vfd_enc;

	struct v4l2_m2m_dev	*m2m_dev_dec;
	struct v4l2_m2m_dev	*m2m_dev_enc;
	struct platform_device	*plat_dev;
	struct vb2_alloc_ctx	*alloc_ctx;
	struct mtk_vcodec_ctx	*ctx[MTK_VCODEC_MAX_INSTANCES];
	int curr_ctx;
	void __iomem		*reg_base[NUM_MAX_VCODEC_REG_BASE];

	unsigned long	instance_mask[BITS_TO_LONGS(MTK_VCODEC_MAX_INSTANCES)];
	int			num_instances;

	struct workqueue_struct *decode_workqueue;
	struct workqueue_struct *encode_workqueue;

	int			int_cond;
	int			int_type;
	struct mutex		dev_mutex;
	wait_queue_head_t	queue;

	int			dec_irq;
	int			enc_irq;
	int			enc_lt_irq;

	struct mutex		dec_mutex;
	struct mutex		enc_mutex;
	unsigned long		enter_suspend;

	struct mtk_vcodec_pm	pm;

	unsigned long		dec_conv;
};

/**
 * struct mtk_vcodec_ctrl - information about controls to be registered.
 * @id:			Control ID.
 * @type:		Type of the control.
 * @name:		Human readable name of the control.
 * @minimum:		Minimum value of the control.
 * @maximum:		Maximum value of the control.
 * @step:		Control value increase step.
 * @menu_skip_mask:	Mask of invalid menu positions.
 * @default_value:	Initial value of the control.
 * @is_volatile:	Control is volatile.
 *
 * See also struct v4l2_ctrl_config.
 */
struct mtk_vcodec_ctrl {
	u32			id;
	enum v4l2_ctrl_type	type;
	u8			name[32];
	s32			minimum;
	s32			maximum;
	s32			step;
	u32			menu_skip_mask;
	s32			default_value;
	u8			is_volatile;
};

static inline struct mtk_vcodec_ctx *fh_to_ctx(struct v4l2_fh *fh)
{
	return container_of(fh, struct mtk_vcodec_ctx, fh);
}

static inline struct mtk_vcodec_ctx *ctrl_to_ctx(struct v4l2_ctrl *ctrl)
{
	return container_of(ctrl->handler, struct mtk_vcodec_ctx, ctrl_hdl);
}

extern const struct v4l2_ioctl_ops mtk_vdec_ioctl_ops;
extern const struct v4l2_m2m_ops mtk_vdec_m2m_ops;
extern const struct v4l2_ioctl_ops mtk_venc_ioctl_ops;
extern const struct v4l2_m2m_ops mtk_venc_m2m_ops;

#endif /* _MEDIA_PLATFORM_MEDIATEK_MTK_VCODEC_DRV_H_ */
