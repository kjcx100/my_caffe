syntax = "proto2";

package caffe;

// Specifies the shape (dimensions) of a Blob.
message BlobShape {
  repeated int64 dim = 1 [packed = true];
}

message BlobProto {
  optional BlobShape shape = 7;
  repeated float data = 5 [packed = true];
  repeated float diff = 6 [packed = true];
  repeated double double_data = 8 [packed = true];
  repeated double double_diff = 9 [packed = true];

  // 4D dimensions -- deprecated.  Use "shape" instead.
  optional int32 num = 1 [default = 0];
  optional int32 channels = 2 [default = 0];
  optional int32 height = 3 [default = 0];
  optional int32 width = 4 [default = 0];
}

// The BlobProtoVector is simply a way to pass multiple blobproto instances
// around.
message BlobProtoVector {
  repeated BlobProto blobs = 1;
}

message Datum {
  optional int32 channels = 1;
  optional int32 height = 2;
  optional int32 width = 3;
  // the actual image data, in bytes
  optional bytes data = 4;
  optional int32 label = 5;
  // Optionally, the datum could also hold float data.
  repeated float float_data = 6;
  // If true data contains an encoded image that need to be decoded
  optional bool encoded = 7 [default = false];
}

message FillerParameter {
  // The filler type.
  optional string type = 1 [default = 'constant'];
  optional float value = 2 [default = 0]; // the value in constant filler
  optional float min = 3 [default = 0]; // the min value in uniform filler
  optional float max = 4 [default = 1]; // the max value in uniform filler
  optional float mean = 5 [default = 0]; // the mean value in Gaussian filler
  optional float std = 6 [default = 1]; // the std value in Gaussian filler
  // The expected number of non-zero output weights for a given input in
  // Gaussian filler -- the default -1 means don't perform sparsification.
  optional int32 sparse = 7 [default = -1];
  // Normalize the filler variance by fan_in, fan_out, or their average.
  // Applies to 'xavier' and 'msra' fillers.
  enum VarianceNorm {
    FAN_IN = 0;
    FAN_OUT = 1;
    AVERAGE = 2;
  }
  optional VarianceNorm variance_norm = 8 [default = FAN_IN];
}

message NetParameter {
  optional string name = 1; // consider giving the network a name
  // DEPRECATED. See InputParameter. The input blobs to the network.
  repeated string input = 3;
  // DEPRECATED. See InputParameter. The shape of the input blobs.
  repeated BlobShape input_shape = 8;

  // 4D input dimensions -- deprecated.  Use "input_shape" instead.
  // If specified, for each input blob there should be four
  // values specifying the num, channels, height and width of the input blob.
  // Thus, there should be a total of (4 * #input) numbers.
  repeated int32 input_dim = 4;

  // Whether the network will force every layer to carry out backward operation.
  // If set False, then whether to carry out backward is determined
  // automatically according to the net structure and learning rates.
  optional bool force_backward = 5 [default = false];
  // The current "state" of the network, including the phase, level, and stage.
  // Some layers may be included/excluded depending on this state and the states
  // specified in the layers' include and exclude fields.
  optional NetState state = 6;

  // Print debugging information about results while running Net::Forward,
  // Net::Backward, and Net::Update.
  optional bool debug_info = 7 [default = false];

  // The layers that make up the net.  Each of their configurations, including
  // connectivity and behavior, is specified as a LayerParameter.
  repeated LayerParameter layer = 100;  // ID 100 so layers are printed last.

  // DEPRECATED: use 'layer' instead.
  repeated V1LayerParameter layers = 2;
}

// NOTE
// Update the next available ID when you add a new SolverParameter field.
//
// SolverParameter next available ID: 43 (last added: weights)
message SolverParameter {
  //////////////////////////////////////////////////////////////////////////////
  // Specifying the train and test networks
  //
  // Exactly one train net must be specified using one of the following fields:
  //     train_net_param, train_net, net_param, net
  // One or more test nets may be specified using any of the following fields:
  //     test_net_param, test_net, net_param, net
  // If more than one test net field is specified (e.g., both net and
  // test_net are specified), they will be evaluated in the field order given
  // above: (1) test_net_param, (2) test_net, (3) net_param/net.
  // A test_iter must be specified for each test_net.
  // A test_level and/or a test_stage may also be specified for each test_net.
  //////////////////////////////////////////////////////////////////////////////

  // Proto filename for the train net, possibly combined with one or more
  // test nets.
  optional string net = 24;
  // Inline train net param, possibly combined with one or more test nets.
  optional NetParameter net_param = 25;

  optional string train_net = 1; // Proto filename for the train net.
  repeated string test_net = 2; // Proto filenames for the test nets.
  optional NetParameter train_net_param = 21; // Inline train net params.
  repeated NetParameter test_net_param = 22; // Inline test net params.

  // The states for the train/test nets. Must be unspecified or
  // specified once per net.
  //
  // By default, train_state will have phase = TRAIN,
  // and all test_state's will have phase = TEST.
  // Other defaults are set according to the NetState defaults.
  optional NetState train_state = 26;
  repeated NetState test_state = 27;

  // The number of iterations for each test net.
  repeated int32 test_iter = 3;

  // The number of iterations between two testing phases.
  optional int32 test_interval = 4 [default = 0];
  optional bool test_compute_loss = 19 [default = false];
  // If true, run an initial test pass before the first iteration,
  // ensuring memory availability and printing the starting value of the loss.
  optional bool test_initialization = 32 [default = true];
  optional float base_lr = 5; // The base learning rate
  // the number of iterations between displaying info. If display = 0, no info
  // will be displayed.
  optional int32 display = 6;
  // Display the loss averaged over the last average_loss iterations
  optional int32 average_loss = 33 [default = 1];
  optional int32 max_iter = 7; // the maximum number of iterations
  // accumulate gradients over `iter_size` x `batch_size` instances
  optional int32 iter_size = 36 [default = 1];

  // The learning rate decay policy. The currently implemented learning rate
  // policies are as follows:
  //    - fixed: always return base_lr.
  //    - step: return base_lr * gamma ^ (floor(iter / step))
  //    - exp: return base_lr * gamma ^ iter
  //    - inv: return base_lr * (1 + gamma * iter) ^ (- power)
  //    - multistep: similar to step but it allows non uniform steps defined by
  //      stepvalue
  //    - poly: the effective learning rate follows a polynomial decay, to be
  //      zero by the max_iter. return base_lr (1 - iter/max_iter) ^ (power)
  //    - sigmoid: the effective learning rate follows a sigmod decay
  //      return base_lr ( 1/(1 + exp(-gamma * (iter - stepsize))))
  //
  // where base_lr, max_iter, gamma, step, stepvalue and power are defined
  // in the solver parameter protocol buffer, and iter is the current iteration.
  optional string lr_policy = 8;
  optional float gamma = 9; // The parameter to compute the learning rate.
  optional float power = 10; // The parameter to compute the learning rate.
  optional float momentum = 11; // The momentum value.
  optional float weight_decay = 12; // The weight decay.
  // regularization types supported: L1 and L2
  // controlled by weight_decay
  optional string regularization_type = 29 [default = "L2"];
  // the stepsize for learning rate policy "step"
  optional int32 stepsize = 13;
  // the stepsize for learning rate policy "multistep"
  repeated int32 stepvalue = 34;

  // Set clip_gradients to >= 0 to clip parameter gradients to that L2 norm,
  // whenever their actual L2 norm is larger.
  optional float clip_gradients = 35 [default = -1];

  optional int32 snapshot = 14 [default = 0]; // The snapshot interval
  // The prefix for the snapshot.
  // If not set then is replaced by prototxt file path without extension.
  // If is set to directory then is augmented by prototxt file name
  // without extention.
  optional string snapshot_prefix = 15;
  // whether to snapshot diff in the results or not. Snapshotting diff will help
  // debugging but the final protocol buffer size will be much larger.
  optional bool snapshot_diff = 16 [default = false];
  enum SnapshotFormat {
    HDF5 = 0;
    BINARYPROTO = 1;
  }
  optional SnapshotFormat snapshot_format = 37 [default = BINARYPROTO];
  // the mode solver will use: 0 for CPU and 1 for GPU. Use GPU in default.
  enum SolverMode {
    CPU = 0;
    GPU = 1;
  }
  optional SolverMode solver_mode = 17 [default = GPU];
  // the device_id will that be used in GPU mode. Use device_id = 0 in default.
  optional int32 device_id = 18 [default = 0];
  // If non-negative, the seed with which the Solver will initialize the Caffe
  // random number generator -- useful for reproducible results. Otherwise,
  // (and by default) initialize using a seed derived from the system clock.
  optional int64 random_seed = 20 [default = -1];

  // type of the solver
  optional string type = 40 [default = "SGD"];

  // numerical stability for RMSProp, AdaGrad and AdaDelta and Adam
  optional float delta = 31 [default = 1e-8];
  // parameters for the Adam solver
  optional float momentum2 = 39 [default = 0.999];

  // RMSProp decay value
  // MeanSquare(t) = rms_decay*MeanSquare(t-1) + (1-rms_decay)*SquareGradient(t)
  optional float rms_decay = 38 [default = 0.99];

  // If true, print information about the state of the net that may help with
  // debugging learning problems.
  optional bool debug_info = 23 [default = false];

  // If false, don't save a snapshot after training finishes.
  optional bool snapshot_after_train = 28 [default = true];

  // DEPRECATED: old solver enum types, use string instead
  enum SolverType {
    SGD = 0;
    NESTEROV = 1;
    ADAGRAD = 2;
    RMSPROP = 3;
    ADADELTA = 4;
    ADAM = 5;
  }
  // DEPRECATED: use type instead of solver_type
  optional SolverType solver_type = 30 [default = SGD];

  // Overlap compute and communication for data parallel training
  optional bool layer_wise_reduce = 41 [default = true];

  // Path to caffemodel file(s) with pretrained weights to initialize finetuning.
  // Tha same as command line --weights parameter for caffe train command.
  // If command line --weights parameter is specified, it has higher priority
  // and overwrites this one(s).
  // If --snapshot command line parameter is specified, this one(s) are ignored.
  // If several model files are expected, they can be listed in a one 
  // weights parameter separated by ',' (like in a command string) or
  // in repeated weights parameters separately.
  repeated string weights = 42;
}

// A message that stores the solver snapshots
message SolverState {
  optional int32 iter = 1; // The current iteration
  optional string learned_net = 2; // The file that stores the learned net.
  repeated BlobProto history = 3; // The history for sgd solvers
  optional int32 current_step = 4 [default = 0]; // The current step for learning rate
}

enum Phase {
   TRAIN = 0;
   TEST = 1;
}

message NetState {
  optional Phase phase = 1 [default = TEST];
  optional int32 level = 2 [default = 0];
  repeated string stage = 3;
}

message NetStateRule {
  // Set phase to require the NetState have a particular phase (TRAIN or TEST)
  // to meet this rule.
  optional Phase phase = 1;

  // Set the minimum and/or maximum levels in which the layer should be used.
  // Leave undefined to meet the rule regardless of level.
  optional int32 min_level = 2;
  optional int32 max_level = 3;

  // Customizable sets of stages to include or exclude.
  // The net must have ALL of the specified stages and NONE of the specified
  // "not_stage"s to meet the rule.
  // (Use multiple NetStateRules to specify conjunctions of stages.)
  repeated string stage = 4;
  repeated string not_stage = 5;
}

// Specifies training parameters (multipliers on global learning constants,
// and the name and other settings used for weight sharing).
message ParamSpec {
  // The names of the parameter blobs -- useful for sharing parameters among
  // layers, but never required otherwise.  To share a parameter between two
  // layers, give it a (non-empty) name.
  optional string name = 1;

  // Whether to require shared weights to have the same shape, or just the same
  // count -- defaults to STRICT if unspecified.
  optional DimCheckMode share_mode = 2;
  enum DimCheckMode {
    // STRICT (default) requires that num, channels, height, width each match.
    STRICT = 0;
    // PERMISSIVE requires only the count (num*channels*height*width) to match.
    PERMISSIVE = 1;
  }

  // The multiplier on the global learning rate for this parameter.
  optional float lr_mult = 3 [default = 1.0];

  // The multiplier on the global weight decay for this parameter.
  optional float decay_mult = 4 [default = 1.0];
}

// NOTE
// Update the next available ID when you add a new LayerParameter field.
//
// LayerParameter next available layer-specific ID: 149 (last added: clip_param)
message LayerParameter {
  optional string name = 1; // the layer name
  optional string type = 2; // the layer type
  repeated string bottom = 3; // the name of each bottom blob
  repeated string top = 4; // the name of each top blob

  // The train / test phase for computation.
  optional Phase phase = 10;

  // The amount of weight to assign each top blob in the objective.
  // Each layer assigns a default value, usually of either 0 or 1,
  // to each top blob.
  repeated float loss_weight = 5;

  // Specifies training parameters (multipliers on global learning constants,
  // and the name and other settings used for weight sharing).
  repeated ParamSpec param = 6;

  // The blobs containing the numeric parameters of the layer.
  repeated BlobProto blobs = 7;

  // Specifies whether to backpropagate to each bottom. If unspecified,
  // Caffe will automatically infer whether each input needs backpropagation
  // to compute parameter gradients. If set to true for some inputs,
  // backpropagation to those inputs is forced; if set false for some inputs,
  // backpropagation to those inputs is skipped.
  //
  // The size must be either 0 or equal to the number of bottoms.
  repeated bool propagate_down = 11;

  // Rules controlling whether and when a layer is included in the network,
  // based on the current NetState.  You may specify a non-zero number of rules
  // to include OR exclude, but not both.  If no include or exclude rules are
  // specified, the layer is always included.  If the current NetState meets
  // ANY (i.e., one or more) of the specified rules, the layer is
  // included/excluded.
  repeated NetStateRule include = 8;
  repeated NetStateRule exclude = 9;

  // Parameters for data pre-processing.
  optional TransformationParameter transform_param = 100;

  // Parameters shared by loss layers.
  optional LossParameter loss_param = 101;

  // Layer type-specific parameters.
  //
  // Note: certain layers may have more than one computational engine
  // for their implementation. These layers include an Engine type and
  // engine parameter for selecting the implementation.
  // The default for the engine is set by the ENGINE switch at compile-time.
  optional AccuracyParameter accuracy_param = 102;
  optional ArgMaxParameter argmax_param = 103;
  optional BatchNormParameter batch_norm_param = 139;
  optional BiasParameter bias_param = 141;
  optional ClipParameter clip_param = 148;
  optional ConcatParameter concat_param = 104;
  optional ContrastiveLossParameter contrastive_loss_param = 105;
  optional ConvolutionParameter convolution_param = 106;
  optional CropParameter crop_param = 144;
  optional DataParameter data_param = 107;
  optional DropoutParameter dropout_param = 108;
  optional DummyDataParameter dummy_data_param = 109;
  optional EltwiseParameter eltwise_param = 110;
  optional ELUParameter elu_param = 140;
  optional EmbedParameter embed_param = 137;
  optional ExpParameter exp_param = 111;
  optional FlattenParameter flatten_param = 135;
  optional HDF5DataParameter hdf5_data_param = 112;
  optional HDF5OutputParameter hdf5_output_param = 113;
  optional HingeLossParameter hinge_loss_param = 114;
  optional ImageDataParameter image_data_param = 115;
  optional InfogainLossParameter infogain_loss_param = 116;
  optional InnerProductParameter inner_product_param = 117;
  optional InputParameter input_param = 143;
  optional LogParameter log_param = 134;
  optional LRNParameter lrn_param = 118;
  optional MemoryDataParameter memory_data_param = 119;
  optional MVNParameter mvn_param = 120;
  optional ParameterParameter parameter_param = 145;
  optional PoolingParameter pooling_param = 121;
  optional PowerParameter power_param = 122;
  optional PReLUParameter prelu_param = 131;
  optional PythonParameter python_param = 130;
  optional RecurrentParameter recurrent_param = 146;
  optional ReductionParameter reduction_param = 136;
  optional ReLUParameter relu_param = 123;
  optional ReshapeParameter reshape_param = 133;
  optional ScaleParameter scale_param = 142;
  optional SigmoidParameter sigmoid_param = 124;
  optional SoftmaxParameter softmax_param = 125;
  optional SPPParameter spp_param = 132;
  optional SliceParameter slice_param = 126;
  optional SwishParameter swish_param = 147;
  optional TanHParameter tanh_param = 127;
  optional ThresholdParameter threshold_param = 128;
  optional TileParameter tile_param = 138;
  optional WindowDataParameter window_data_param = 129;
}

// Message that stores parameters used to apply transformation
// to the data layer's data
message TransformationParameter {
  // For data pre-processing, we can do simple scaling and subtracting the
  // data mean, if provided. Note that the mean subtraction is always carried
  // out before scaling.
  optional float scale = 1 [default = 1];
  // Specify if we want to randomly mirror data.
  optional bool mirror = 2 [default = false];
  // Specify if we would like to randomly crop an image.
  optional uint32 crop_size = 3 [default = 0];
  // mean_file and mean_value cannot be specified at the same time
  optional string mean_file = 4;
  // if specified can be repeated once (would subtract it from all the channels)
  // or can be repeated the same number of times as channels
  // (would subtract them from the corresponding channel)
  repeated float mean_value = 5;
  // Force the decoded image to have 3 color channels.
  optional bool force_color = 6 [default = false];
  // Force the decoded image to have 1 color channels.
  optional bool force_gray = 7 [default = false];
}

// Message that stores parameters shared by loss layers
message LossParameter {
  // If specified, ignore instances with the given label.
  optional int32 ignore_label = 1;
  // How to normalize the loss for loss layers that aggregate across batches,
  // spatial dimensions, or other dimensions.  Currently only implemented in
  // SoftmaxWithLoss and SigmoidCrossEntropyLoss layers.
  enum NormalizationMode {
    // Divide by the number of examples in the batch times spatial dimensions.
    // Outputs that receive the ignore label will NOT be ignored in computing
    // the normalization factor.
    FULL = 0;
    // Divide by the total number of output locations that do not take the
    // ignore_label.  If ignore_label is not set, this behaves like FULL.
    VALID = 1;
    // Divide by the batch size.
    BATCH_SIZE = 2;
    // Do not normalize the loss.
    NONE = 3;
  }
  // For historical reasons, the default normalization for
  // SigmoidCrossEntropyLoss is BATCH_SIZE and *not* VALID.
  optional NormalizationMode normalization = 3 [default = VALID];
  // Deprecated.  Ignored if normalization is specified.  If normalization
  // is not specified, then setting this to false will be equivalent to
  // normalization = BATCH_SIZE to be consistent with previous behavior.
  optional bool normalize = 2;
}

// Messages that store parameters used by individual layer types follow, in
// alphabetical order.

message AccuracyParameter {
  // When computing accuracy, count as correct by comparing the true label to
  // the top k scoring classes.  By default, only compare to the top scoring
  // class (i.e. argmax).
  optional uint32 top_k = 1 [default = 1];

  // The "label" axis of the prediction blob, whose argmax corresponds to the
  // predicted label -- may be negative to index from the end (e.g., -1 for the
  // last axis).  For example, if axis == 1 and the predictions are
  // (N x C x H x W), the label blob is expected to contain N*H*W ground truth
  // labels with integer values in {0, 1, ..., C-1}.
  optional int32 axis = 2 [default = 1];

  // If specified, ignore instances with the given label.
  optional int32 ignore_label = 3;
}

message ArgMaxParameter {
  // If true produce pairs (argmax, maxval)
  optional bool out_max_val = 1 [default = false];
  optional uint32 top_k = 2 [default = 1];
  // The axis along which to maximise -- may be negative to index from the
  // end (e.g., -1 for the last axis).
  // By default ArgMaxLayer maximizes over the flattened trailing dimensions
  // for each index of the first / num dimension.
  optional int32 axis = 3;
}

// Message that stores parameters used by ClipLayer
message ClipParameter {
  required float min = 1;
  required float max = 2;
}

message ConcatParameter {
  // The axis along which to concatenate -- may be negative to index from the
  // end (e.g., -1 for the last axis).  Other axes must have the
  // same dimension for all the bottom blobs.
  // By default, ConcatLayer concatenates blobs along the "channels" axis (1).
  optional int32 axis = 2 [default = 1];

  // DEPRECATED: alias for "axis" -- does not support negative indexing.
  optional uint32 concat_dim = 1 [default = 1];
}

message BatchNormParameter {
  // If false, normalization is performed over the current mini-batch
  // and global statistics are accumulated (but not yet used) by a moving
  // average.
  // If true, those accumulated mean and variance values are used for the
  // normalization.
  // By default, it is set to false when the network is in the training
  // phase and true when the network is in the testing phase.
  optional bool use_global_stats = 1;
  // What fraction of the moving average remains each iteration?
  // Smaller values make the moving average decay faster, giving more
  // weight to the recent values.
  // Each iteration updates the moving average @f$S_{t-1}@f$ with the
  // current mean @f$ Y_t @f$ by
  // @f$ S_t = (1-\beta)Y_t + \beta \cdot S_{t-1} @f$, where @f$ \beta @f$
  // is the moving_average_fraction parameter.
  optional float moving_average_fraction = 2 [default = .999];
  // Small value to add to the variance estimate so that we don't divide by
  // zero.
  optional float eps = 3 [default = 1e-5];
}

message BiasParameter {
  // The first axis of bottom[0] (the first input Blob) along which to apply
  // bottom[1] (the second input Blob).  May be negative to index from the end
  // (e.g., -1 for the last axis).
  //
  // For example, if bottom[0] is 4D with shape 100x3x40x60, the output
  // top[0] will have the same shape, and bottom[1] may have any of the
  // following shapes (for the given value of axis):
  //    (axis == 0 == -4) 100; 100x3; 100x3x40; 100x3x40x60
  //    (axis == 1 == -3)          3;     3x40;     3x40x60
  //    (axis == 2 == -2)                   40;       40x60
  //    (axis == 3 == -1)                                60
  // Furthermore, bottom[1] may have the empty shape (regardless of the value of
  // "axis") -- a scalar bias.
  optional int32 axis = 1 [default = 1];

  // (num_axes is ignored unless just one bottom is given and the bias is
  // a learned parameter of the layer.  Otherwise, num_axes is determined by the
  // number of axes by the second bottom.)
  // The number of axes of the input (bottom[0]) covered by the bias
  // parameter, or -1 to cover all axes of bottom[0] starting from `axis`.
  // Set num_axes := 0, to add a zero-axis Blob: a scalar.
  optional int32 num_axes = 2 [default = 1];

  // (filler is ignored unless just one bottom is given and the bias is
  // a learned parameter of the layer.)
  // The initialization for the learned bias parameter.
  // Default is the zero (0) initialization, resulting in the BiasLayer
  // initially performing the identity operation.
  optional FillerParameter filler = 3;
}

message ContrastiveLossParameter {
  // margin for dissimilar pair
  optional float margin = 1 [default = 1.0];
  // The first implementation of this cost did not exactly match the cost of
  // Hadsell et al 2006 -- using (margin - d^2) instead of (margin - d)^2.
  // legacy_version = false (the default) uses (margin - d)^2 as proposed in the
  // Hadsell paper. New models should probably use this version.
  // legacy_version = true uses (margin - d^2). This is kept to support /
  // reproduce existing models and results
  optional bool legacy_version = 2 [default = false];
}

message ConvolutionParameter {
  optional uint32 num_output = 1; // The number of outputs for the layer
  optional bool bias_term = 2 [default = true]; // whether to have bias terms

  // Pad, kernel size, and stride are all given as a single value for equal
  // dimensions in all spatial dimensions, or once per spatial dimension.
  repeated uint32 pad = 3; // The padding size; defaults to 0
  repeated uint32 kernel_size = 4; // The kernel size
  repeated uint32 stride = 6; // The stride; defaults to 1
  // Factor used to dilate the kernel, (implicitly) zero-filling the resulting
  // holes. (Kernel dilation is sometimes referred to by its use in the
  // algorithme à trous from Holschneider et al. 1987.)
  repeated uint32 dilation = 18; // The dilation; defaults to 1

  // For 2D convolution only, the *_h and *_w versions may also be used to
  // specify both spatial dimensions.
  optional uint32 pad_h = 9 [default = 0]; // The padding height (2D only)
  optional uint32 pad_w = 10 [default = 0]; // The padding width (2D only)
  optional uint32 kernel_h = 11; // The kernel height (2D only)
  optional uint32 kernel_w = 12; // The kernel width (2D only)
  optional uint32 stride_h = 13; // The stride height (2D only)
  optional uint32 stride_w = 14; // The stride width (2D only)

  optional uint32 group = 5 [default = 1]; // The group size for group conv

  optional FillerParameter weight_filler = 7; // The filler for the weight
  optional FillerParameter bias_filler = 8; // The filler for the bias
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 15 [default = DEFAULT];

  // The axis to interpret as "channels" when performing convolution.
  // Preceding dimensions are treated as independent inputs;
  // succeeding dimensions are treated as "spatial".
  // With (N, C, H, W) inputs, and axis == 1 (the default), we perform
  // N independent 2D convolutions, sliding C-channel (or (C/g)-channels, for
  // groups g>1) filters across the spatial axes (H, W) of the input.
  // With (N, C, D, H, W) inputs, and axis == 1, we perform
  // N independent 3D convolutions, sliding (C/g)-channels
  // filters across the spatial axes (D, H, W) of the input.
  optional int32 axis = 16 [default = 1];

  // Whether to force use of the general ND convolution, even if a specific
  // implementation for blobs of the appropriate number of spatial dimensions
  // is available. (Currently, there is only a 2D-specific convolution
  // implementation; for input blobs with num_axes != 2, this option is
  // ignored and the ND implementation will be used.)
  optional bool force_nd_im2col = 17 [default = false];
}

message CropParameter {
  // To crop, elements of the first bottom are selected to fit the dimensions
  // of the second, reference bottom. The crop is configured by
  // - the crop `axis` to pick the dimensions for cropping
  // - the crop `offset` to set the shift for all/each dimension
  // to align the cropped bottom with the reference bottom.
  // All dimensions up to but excluding `axis` are preserved, while
  // the dimensions including and trailing `axis` are cropped.
  // If only one `offset` is set, then all dimensions are offset by this amount.
  // Otherwise, the number of offsets must equal the number of cropped axes to
  // shift the crop in each dimension accordingly.
  // Note: standard dimensions are N,C,H,W so the default is a spatial crop,
  // and `axis` may be negative to index from the end (e.g., -1 for the last
  // axis).
  optional int32 axis = 1 [default = 2];
  repeated uint32 offset = 2;
}

message DataParameter {
  enum DB {
    LEVELDB = 0;
    LMDB = 1;
  }
  // Specify the data source.
  optional string source = 1;
  // Specify the batch size.
  optional uint32 batch_size = 4;
  // The rand_skip variable is for the data layer to skip a few data points
  // to avoid all asynchronous sgd clients to start at the same point. The skip
  // point would be set as rand_skip * rand(0,1). Note that rand_skip should not
  // be larger than the number of keys in the database.
  // DEPRECATED. Each solver accesses a different subset of the database.
  optional uint32 rand_skip = 7 [default = 0];
  optional DB backend = 8 [default = LEVELDB];
  // DEPRECATED. See TransformationParameter. For data pre-processing, we can do
  // simple scaling and subtracting the data mean, if provided. Note that the
  // mean subtraction is always carried out before scaling.
  optional float scale = 2 [default = 1];
  optional string mean_file = 3;
  // DEPRECATED. See TransformationParameter. Specify if we would like to randomly
  // crop an image.
  optional uint32 crop_size = 5 [default = 0];
  // DEPRECATED. See TransformationParameter. Specify if we want to randomly mirror
  // data.
  optional bool mirror = 6 [default = false];
  // Force the encoded image to have 3 color channels
  optional bool force_encoded_color = 9 [default = false];
  // Prefetch queue (Increase if data feeding bandwidth varies, within the
  // limit of device memory for GPU training)
  optional uint32 prefetch = 10 [default = 4];
}

message DropoutParameter {
  optional float dropout_ratio = 1 [default = 0.5]; // dropout ratio
}

// DummyDataLayer fills any number of arbitrarily shaped blobs with random
// (or constant) data generated by "Fillers" (see "message FillerParameter").
message DummyDataParameter {
  // This layer produces N >= 1 top blobs.  DummyDataParameter must specify 1 or N
  // shape fields, and 0, 1 or N data_fillers.
  //
  // If 0 data_fillers are specified, ConstantFiller with a value of 0 is used.
  // If 1 data_filler is specified, it is applied to all top blobs.  If N are
  // specified, the ith is applied to the ith top blob.
  repeated FillerParameter data_filler = 1;
  repeated BlobShape shape = 6;

  // 4D dimensions -- deprecated.  Use "shape" instead.
  repeated uint32 num = 2;
  repeated uint32 channels = 3;
  repeated uint32 height = 4;
  repeated uint32 width = 5;
}

message EltwiseParameter {
  enum EltwiseOp {
    PROD = 0;
    SUM = 1;
    MAX = 2;
  }
  optional EltwiseOp operation = 1 [default = SUM]; // element-wise operation
  repeated float coeff = 2; // blob-wise coefficient for SUM operation

  // Whether to use an asymptotically slower (for >2 inputs) but stabler method
  // of computing the gradient for the PROD operation. (No effect for SUM op.)
  optional bool stable_prod_grad = 3 [default = true];
}

// Message that stores parameters used by ELULayer
message ELUParameter {
  // Described in:
  // Clevert, D.-A., Unterthiner, T., & Hochreiter, S. (2015). Fast and Accurate
  // Deep Network Learning by Exponential Linear Units (ELUs). arXiv
  optional float alpha = 1 [default = 1];
}

// Message that stores parameters used by EmbedLayer
message EmbedParameter {
  optional uint32 num_output = 1; // The number of outputs for the layer
  // The input is given as integers to be interpreted as one-hot
  // vector indices with dimension num_input.  Hence num_input should be
  // 1 greater than the maximum possible input value.
  optional uint32 input_dim = 2;

  optional bool bias_term = 3 [default = true]; // Whether to use a bias term
  optional FillerParameter weight_filler = 4; // The filler for the weight
  optional FillerParameter bias_filler = 5; // The filler for the bias

}

// Message that stores parameters used by ExpLayer
message ExpParameter {
  // ExpLayer computes outputs y = base ^ (shift + scale * x), for base > 0.
  // Or if base is set to the default (-1), base is set to e,
  // so y = exp(shift + scale * x).
  optional float base = 1 [default = -1.0];
  optional float scale = 2 [default = 1.0];
  optional float shift = 3 [default = 0.0];
}

/// Message that stores parameters used by FlattenLayer
message FlattenParameter {
  // The first axis to flatten: all preceding axes are retained in the output.
  // May be negative to index from the end (e.g., -1 for the last axis).
  optional int32 axis = 1 [default = 1];

  // The last axis to flatten: all following axes are retained in the output.
  // May be negative to index from the end (e.g., the default -1 for the last
  // axis).
  optional int32 end_axis = 2 [default = -1];
}

// Message that stores parameters used by HDF5DataLayer
message HDF5DataParameter {
  // Specify the data source.
  optional string source = 1;
  // Specify the batch size.
  optional uint32 batch_size = 2;

  // Specify whether to shuffle the data.
  // If shuffle == true, the ordering of the HDF5 files is shuffled,
  // and the ordering of data within any given HDF5 file is shuffled,
  // but data between different files are not interleaved; all of a file's
  // data are output (in a random order) before moving onto another file.
  optional bool shuffle = 3 [default = false];
}

message HDF5OutputParameter {
  optional string file_name = 1;
}

message HingeLossParameter {
  enum Norm {
    L1 = 1;
    L2 = 2;
  }
  // Specify the Norm to use L1 or L2
  optional Norm norm = 1 [default = L1];
}

message ImageDataParameter {
  // Specify the data source.
  optional string source = 1;
  // Specify the batch size.
  optional uint32 batch_size = 4 [default = 1];
  // The rand_skip variable is for the data layer to skip a few data points
  // to avoid all asynchronous sgd clients to start at the same point. The skip
  // point would be set as rand_skip * rand(0,1). Note that rand_skip should not
  // be larger than the number of keys in the database.
  optional uint32 rand_skip = 7 [default = 0];
  // Whether or not ImageLayer should shuffle the list of files at every epoch.
  optional bool shuffle = 8 [default = false];
  // It will also resize images if new_height or new_width are not zero.
  optional uint32 new_height = 9 [default = 0];
  optional uint32 new_width = 10 [default = 0];
  // Specify if the images are color or gray
  optional bool is_color = 11 [default = true];
  // DEPRECATED. See TransformationParameter. For data pre-processing, we can do
  // simple scaling and subtracting the data mean, if provided. Note that the
  // mean subtraction is always carried out before scaling.
  optional float scale = 2 [default = 1];
  optional string mean_file = 3;
  // DEPRECATED. See TransformationParameter. Specify if we would like to randomly
  // crop an image.
  optional uint32 crop_size = 5 [default = 0];
  // DEPRECATED. See TransformationParameter. Specify if we want to randomly mirror
  // data.
  optional bool mirror = 6 [default = false];
  optional string root_folder = 12 [default = ""];
}

message InfogainLossParameter {
  // Specify the infogain matrix source.
  optional string source = 1;
  optional int32 axis = 2 [default = 1]; // axis of prob
}

message InnerProductParameter {
  optional uint32 num_output = 1; // The number of outputs for the layer
  optional bool bias_term = 2 [default = true]; // whether to have bias terms
  optional FillerParameter weight_filler = 3; // The filler for the weight
  optional FillerParameter bias_filler = 4; // The filler for the bias

  // The first axis to be lumped into a single inner product computation;
  // all preceding axes are retained in the output.
  // May be negative to index from the end (e.g., -1 for the last axis).
  optional int32 axis = 5 [default = 1];
  // Specify whether to transpose the weight matrix or not.
  // If transpose == true, any operations will be performed on the transpose
  // of the weight matrix. The weight matrix itself is not going to be transposed
  // but rather the transfer flag of operations will be toggled accordingly.
  optional bool transpose = 6 [default = false];
}

message InputParameter {
  // This layer produces N >= 1 top blob(s) to be assigned manually.
  // Define N shapes to set a shape for each top.
  // Define 1 shape to set the same shape for every top.
  // Define no shape to defer to reshaping manually.
  repeated BlobShape shape = 1;
}

// Message that stores parameters used by LogLayer
message LogParameter {
  // LogLayer computes outputs y = log_base(shift + scale * x), for base > 0.
  // Or if base is set to the default (-1), base is set to e,
  // so y = ln(shift + scale * x) = log_e(shift + scale * x)
  optional float base = 1 [default = -1.0];
  optional float scale = 2 [default = 1.0];
  optional float shift = 3 [default = 0.0];
}

// Message that stores parameters used by LRNLayer
message LRNParameter {
  optional uint32 local_size = 1 [default = 5];
  optional float alpha = 2 [default = 1.];
  optional float beta = 3 [default = 0.75];
  enum NormRegion {
    ACROSS_CHANNELS = 0;
    WITHIN_CHANNEL = 1;
  }
  optional NormRegion norm_region = 4 [default = ACROSS_CHANNELS];
  optional float k = 5 [default = 1.];
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 6 [default = DEFAULT];
}

message MemoryDataParameter {
  optional uint32 batch_size = 1;
  optional uint32 channels = 2;
  optional uint32 height = 3;
  optional uint32 width = 4;
}

message MVNParameter {
  // This parameter can be set to false to normalize mean only
  optional bool normalize_variance = 1 [default = true];

  // This parameter can be set to true to perform DNN-like MVN
  optional bool across_channels = 2 [default = false];

  // Epsilon for not dividing by zero while normalizing variance
  optional float eps = 3 [default = 1e-9];
}

message ParameterParameter {
  optional BlobShape shape = 1;
}

message PoolingParameter {
  enum PoolMethod {
    MAX = 0;
    AVE = 1;
    STOCHASTIC = 2;
  }
  optional PoolMethod pool = 1 [default = MAX]; // The pooling method
  // Pad, kernel size, and stride are all given as a single value for equal
  // dimensions in height and width or as Y, X pairs.
  optional uint32 pad = 4 [default = 0]; // The padding size (equal in Y, X)
  optional uint32 pad_h = 9 [default = 0]; // The padding height
  optional uint32 pad_w = 10 [default = 0]; // The padding width
  optional uint32 kernel_size = 2; // The kernel size (square)
  optional uint32 kernel_h = 5; // The kernel height
  optional uint32 kernel_w = 6; // The kernel width
  optional uint32 stride = 3 [default = 1]; // The stride (equal in Y, X)
  optional uint32 stride_h = 7; // The stride height
  optional uint32 stride_w = 8; // The stride width
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 11 [default = DEFAULT];
  // If global_pooling then it will pool over the size of the bottom by doing
  // kernel_h = bottom->height and kernel_w = bottom->width
  optional bool global_pooling = 12 [default = false];
  // How to calculate the output size - using ceil (default) or floor rounding.
  enum RoundMode {
    CEIL = 0;
    FLOOR = 1;
  }
  optional RoundMode round_mode = 13 [default = CEIL];
}

message PowerParameter {
  // PowerLayer computes outputs y = (shift + scale * x) ^ power.
  optional float power = 1 [default = 1.0];
  optional float scale = 2 [default = 1.0];
  optional float shift = 3 [default = 0.0];
}

message PythonParameter {
  optional string module = 1;
  optional string layer = 2;
  // This value is set to the attribute `param_str` of the `PythonLayer` object
  // in Python before calling the `setup()` method. This could be a number,
  // string, dictionary in Python dict format, JSON, etc. You may parse this
  // string in `setup` method and use it in `forward` and `backward`.
  optional string param_str = 3 [default = ''];
  // DEPRECATED
  optional bool share_in_parallel = 4 [default = false];
}

// Message that stores parameters used by RecurrentLayer
message RecurrentParameter {
  // The dimension of the output (and usually hidden state) representation --
  // must be explicitly set to non-zero.
  optional uint32 num_output = 1 [default = 0];

  optional FillerParameter weight_filler = 2; // The filler for the weight
  optional FillerParameter bias_filler = 3; // The filler for the bias

  // Whether to enable displaying debug_info in the unrolled recurrent net.
  optional bool debug_info = 4 [default = false];

  // Whether to add as additional inputs (bottoms) the initial hidden state
  // blobs, and add as additional outputs (tops) the final timestep hidden state
  // blobs.  The number of additional bottom/top blobs required depends on the
  // recurrent architecture -- e.g., 1 for RNNs, 2 for LSTMs.
  optional bool expose_hidden = 5 [default = false];
}

// Message that stores parameters used by ReductionLayer
message ReductionParameter {
  enum ReductionOp {
    SUM = 1;
    ASUM = 2;
    SUMSQ = 3;
    MEAN = 4;
  }

  optional ReductionOp operation = 1 [default = SUM]; // reduction operation

  // The first axis to reduce to a scalar -- may be negative to index from the
  // end (e.g., -1 for the last axis).
  // (Currently, only reduction along ALL "tail" axes is supported; reduction
  // of axis M through N, where N < num_axes - 1, is unsupported.)
  // Suppose we have an n-axis bottom Blob with shape:
  //     (d0, d1, d2, ..., d(m-1), dm, d(m+1), ..., d(n-1)).
  // If axis == m, the output Blob will have shape
  //     (d0, d1, d2, ..., d(m-1)),
  // and the ReductionOp operation is performed (d0 * d1 * d2 * ... * d(m-1))
  // times, each including (dm * d(m+1) * ... * d(n-1)) individual data.
  // If axis == 0 (the default), the output Blob always has the empty shape
  // (count 1), performing reduction across the entire input --
  // often useful for creating new loss functions.
  optional int32 axis = 2 [default = 0];

  optional float coeff = 3 [default = 1.0]; // coefficient for output
}

// Message that stores parameters used by ReLULayer
message ReLUParameter {
  // Allow non-zero slope for negative inputs to speed up optimization
  // Described in:
  // Maas, A. L., Hannun, A. Y., & Ng, A. Y. (2013). Rectifier nonlinearities
  // improve neural network acoustic models. In ICML Workshop on Deep Learning
  // for Audio, Speech, and Language Processing.
  optional float negative_slope = 1 [default = 0];
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 2 [default = DEFAULT];
}

message ReshapeParameter {
  // Specify the output dimensions. If some of the dimensions are set to 0,
  // the corresponding dimension from the bottom layer is used (unchanged).
  // Exactly one dimension may be set to -1, in which case its value is
  // inferred from the count of the bottom blob and the remaining dimensions.
  // For example, suppose we want to reshape a 2D blob "input" with shape 2 x 8:
  //
  //   layer {
  //     type: "Reshape" bottom: "input" top: "output"
  //     reshape_param { ... }
  //   }
  //
  // If "input" is 2D with shape 2 x 8, then the following reshape_param
  // specifications are all equivalent, producing a 3D blob "output" with shape
  // 2 x 2 x 4:
  //
  //   reshape_param { shape { dim:  2  dim: 2  dim:  4 } }
  //   reshape_param { shape { dim:  0  dim: 2  dim:  4 } }
  //   reshape_param { shape { dim:  0  dim: 2  dim: -1 } }
  //   reshape_param { shape { dim:  0  dim:-1  dim:  4 } }
  //
  optional BlobShape shape = 1;

  // axis and num_axes control the portion of the bottom blob's shape that are
  // replaced by (included in) the reshape. By default (axis == 0 and
  // num_axes == -1), the entire bottom blob shape is included in the reshape,
  // and hence the shape field must specify the entire output shape.
  //
  // axis may be non-zero to retain some portion of the beginning of the input
  // shape (and may be negative to index from the end; e.g., -1 to begin the
  // reshape after the last axis, including nothing in the reshape,
  // -2 to include only the last axis, etc.).
  //
  // For example, suppose "input" is a 2D blob with shape 2 x 8.
  // Then the following ReshapeLayer specifications are all equivalent,
  // producing a blob "output" with shape 2 x 2 x 4:
  //
  //   reshape_param { shape { dim: 2  dim: 2  dim: 4 } }
  //   reshape_param { shape { dim: 2  dim: 4 } axis:  1 }
  //   reshape_param { shape { dim: 2  dim: 4 } axis: -3 }
  //
  // num_axes specifies the extent of the reshape.
  // If num_axes >= 0 (and axis >= 0), the reshape will be performed only on
  // input axes in the range [axis, axis+num_axes].
  // num_axes may also be -1, the default, to include all remaining axes
  // (starting from axis).
  //
  // For example, suppose "input" is a 2D blob with shape 2 x 8.
  // Then the following ReshapeLayer specifications are equivalent,
  // producing a blob "output" with shape 1 x 2 x 8.
  //
  //   reshape_param { shape { dim:  1  dim: 2  dim:  8 } }
  //   reshape_param { shape { dim:  1  dim: 2  }  num_axes: 1 }
  //   reshape_param { shape { dim:  1  }  num_axes: 0 }
  //
  // On the other hand, these would produce output blob shape 2 x 1 x 8:
  //
  //   reshape_param { shape { dim: 2  dim: 1  dim: 8  }  }
  //   reshape_param { shape { dim: 1 }  axis: 1  num_axes: 0 }
  //
  optional int32 axis = 2 [default = 0];
  optional int32 num_axes = 3 [default = -1];
}

message ScaleParameter {
  // The first axis of bottom[0] (the first input Blob) along which to apply
  // bottom[1] (the second input Blob).  May be negative to index from the end
  // (e.g., -1 for the last axis).
  //
  // For example, if bottom[0] is 4D with shape 100x3x40x60, the output
  // top[0] will have the same shape, and bottom[1] may have any of the
  // following shapes (for the given value of axis):
  //    (axis == 0 == -4) 100; 100x3; 100x3x40; 100x3x40x60
  //    (axis == 1 == -3)          3;     3x40;     3x40x60
  //    (axis == 2 == -2)                   40;       40x60
  //    (axis == 3 == -1)                                60
  // Furthermore, bottom[1] may have the empty shape (regardless of the value of
  // "axis") -- a scalar multiplier.
  optional int32 axis = 1 [default = 1];

  // (num_axes is ignored unless just one bottom is given and the scale is
  // a learned parameter of the layer.  Otherwise, num_axes is determined by the
  // number of axes by the second bottom.)
  // The number of axes of the input (bottom[0]) covered by the scale
  // parameter, or -1 to cover all axes of bottom[0] starting from `axis`.
  // Set num_axes := 0, to multiply with a zero-axis Blob: a scalar.
  optional int32 num_axes = 2 [default = 1];

  // (filler is ignored unless just one bottom is given and the scale is
  // a learned parameter of the layer.)
  // The initialization for the learned scale parameter.
  // Default is the unit (1) initialization, resulting in the ScaleLayer
  // initially performing the identity operation.
  optional FillerParameter filler = 3;

  // Whether to also learn a bias (equivalent to a ScaleLayer+BiasLayer, but
  // may be more efficient).  Initialized with bias_filler (defaults to 0).
  optional bool bias_term = 4 [default = false];
  optional FillerParameter bias_filler = 5;
}

message SigmoidParameter {
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 1 [default = DEFAULT];
}

message SliceParameter {
  // The axis along which to slice -- may be negative to index from the end
  // (e.g., -1 for the last axis).
  // By default, SliceLayer concatenates blobs along the "channels" axis (1).
  optional int32 axis = 3 [default = 1];
  repeated uint32 slice_point = 2;

  // DEPRECATED: alias for "axis" -- does not support negative indexing.
  optional uint32 slice_dim = 1 [default = 1];
}

// Message that stores parameters used by SoftmaxLayer, SoftmaxWithLossLayer
message SoftmaxParameter {
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 1 [default = DEFAULT];

  // The axis along which to perform the softmax -- may be negative to index
  // from the end (e.g., -1 for the last axis).
  // Any other axes will be evaluated as independent softmaxes.
  optional int32 axis = 2 [default = 1];
}

// Message that stores parameters used by SwishLayer
message SwishParameter {
  // Beta parameter for the Swish activation function
  // Described in:
  // Prajit Ramachandran, Barret Zoph, Quoc V. Le. (2017). Searching for
  // Activation Functions. https://arxiv.org/abs/1710.05941v2
  optional float beta = 1 [default = 1];
}

message TanHParameter {
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 1 [default = DEFAULT];
}

// Message that stores parameters used by TileLayer
message TileParameter {
  // The index of the axis to tile.
  optional int32 axis = 1 [default = 1];

  // The number of copies (tiles) of the blob to output.
  optional int32 tiles = 2;
}

// Message that stores parameters used by ThresholdLayer
message ThresholdParameter {
  optional float threshold = 1 [default = 0]; // Strictly positive values
}

message WindowDataParameter {
  // Specify the data source.
  optional string source = 1;
  // For data pre-processing, we can do simple scaling and subtracting the
  // data mean, if provided. Note that the mean subtraction is always carried
  // out before scaling.
  optional float scale = 2 [default = 1];
  optional string mean_file = 3;
  // Specify the batch size.
  optional uint32 batch_size = 4;
  // Specify if we would like to randomly crop an image.
  optional uint32 crop_size = 5 [default = 0];
  // Specify if we want to randomly mirror data.
  optional bool mirror = 6 [default = false];
  // Foreground (object) overlap threshold
  optional float fg_threshold = 7 [default = 0.5];
  // Background (non-object) overlap threshold
  optional float bg_threshold = 8 [default = 0.5];
  // Fraction of batch that should be foreground objects
  optional float fg_fraction = 9 [default = 0.25];
  // Amount of contextual padding to add around a window
  // (used only by the window_data_layer)
  optional uint32 context_pad = 10 [default = 0];
  // Mode for cropping out a detection window
  // warp: cropped window is warped to a fixed size and aspect ratio
  // square: the tightest square around the window is cropped
  optional string crop_mode = 11 [default = "warp"];
  // cache_images: will load all images in memory for faster access
  optional bool cache_images = 12 [default = false];
  // append root_folder to locate images
  optional string root_folder = 13 [default = ""];
}

message SPPParameter {
  enum PoolMethod {
    MAX = 0;
    AVE = 1;
    STOCHASTIC = 2;
  }
  optional uint32 pyramid_height = 1;
  optional PoolMethod pool = 2 [default = MAX]; // The pooling method
  enum Engine {
    DEFAULT = 0;
    CAFFE = 1;
    CUDNN = 2;
  }
  optional Engine engine = 6 [default = DEFAULT];
}

// DEPRECATED: use LayerParameter.
message V1LayerParameter {
  repeated string bottom = 2;
  repeated string top = 3;
  optional string name = 4;
  repeated NetStateRule include = 32;
  repeated NetStateRule exclude = 33;
  enum LayerType {
    NONE = 0;
    ABSVAL = 35;
    ACCURACY = 1;
    ARGMAX = 30;
    BNLL = 2;
    CONCAT = 3;
    CONTRASTIVE_LOSS = 37;
    CONVOLUTION = 4;
    DATA = 5;
    DECONVOLUTION = 39;
    DROPOUT = 6;
    DUMMY_DATA = 32;
    EUCLIDEAN_LOSS = 7;
    ELTWISE = 25;
    EXP = 38;
    FLATTEN = 8;
    HDF5_DATA = 9;
    HDF5_OUTPUT = 10;
    HINGE_LOSS = 28;
    IM2COL = 11;
    IMAGE_DATA = 12;
    INFOGAIN_LOSS = 13;
    INNER_PRODUCT = 14;
    LRN = 15;
    MEMORY_DATA = 29;
    MULTINOMIAL_LOGISTIC_LOSS = 16;
    MVN = 34;
    POOLING = 17;
    POWER = 26;
    RELU = 18;
    SIGMOID = 19;
    SIGMOID_CROSS_ENTROPY_LOSS = 27;
    SILENCE = 36;
    SOFTMAX = 20;
    SOFTMAX_LOSS = 21;
    SPLIT = 22;
    SLICE = 33;
    TANH = 23;
    WINDOW_DATA = 24;
    THRESHOLD = 31;
  }
  optional LayerType type = 5;
  repeated BlobProto blobs = 6;
  repeated string param = 1001;
  repeated DimCheckMode blob_share_mode = 1002;
  enum DimCheckMode {
    STRICT = 0;
    PERMISSIVE = 1;
  }
  repeated float blobs_lr = 7;
  repeated float weight_decay = 8;
  repeated float loss_weight = 35;
  optional AccuracyParameter accuracy_param = 27;
  optional ArgMaxParameter argmax_param = 23;
  optional ConcatParameter concat_param = 9;
  optional ContrastiveLossParameter contrastive_loss_param = 40;
  optional ConvolutionParameter convolution_param = 10;
  optional DataParameter data_param = 11;
  optional DropoutParameter dropout_param = 12;
  optional DummyDataParameter dummy_data_param = 26;
  optional EltwiseParameter eltwise_param = 24;
  optional ExpParameter exp_param = 41;
  optional HDF5DataParameter hdf5_data_param = 13;
  optional HDF5OutputParameter hdf5_output_param = 14;
  optional HingeLossParameter hinge_loss_param = 29;
  optional ImageDataParameter image_data_param = 15;
  optional InfogainLossParameter infogain_loss_param = 16;
  optional InnerProductParameter inner_product_param = 17;
  optional LRNParameter lrn_param = 18;
  optional MemoryDataParameter memory_data_param = 22;
  optional MVNParameter mvn_param = 34;
  optional PoolingParameter pooling_param = 19;
  optional PowerParameter power_param = 21;
  optional ReLUParameter relu_param = 30;
  optional SigmoidParameter sigmoid_param = 38;
  optional SoftmaxParameter softmax_param = 39;
  optional SliceParameter slice_param = 31;
  optional TanHParameter tanh_param = 37;
  optional ThresholdParameter threshold_param = 25;
  optional WindowDataParameter window_data_param = 20;
  optional TransformationParameter transform_param = 36;
  optional LossParameter loss_param = 42;
  optional V0LayerParameter layer = 1;
}

// DEPRECATED: V0LayerParameter is the old way of specifying layer parameters
// in Caffe.  We keep this message type around for legacy support.
message V0LayerParameter {
  optional string name = 1; // the layer name
  optional string type = 2; // the string to specify the layer type

  // Parameters to specify layers with inner products.
  optional uint32 num_output = 3; // The number of outputs for the layer
  optional bool biasterm = 4 [default = true]; // whether to have bias terms
  optional FillerParameter weight_filler = 5; // The filler for the weight
  optional FillerParameter bias_filler = 6; // The filler for the bias

  optional uint32 pad = 7 [default = 0]; // The padding size
  optional uint32 kernelsize = 8; // The kernel size
  optional uint32 group = 9 [default = 1]; // The group size for group conv
  optional uint32 stride = 10 [default = 1]; // The stride
  enum PoolMethod {
    MAX = 0;
    AVE = 1;
    STOCHASTIC = 2;
  }
  optional PoolMethod pool = 11 [default = MAX]; // The pooling method
  optional float dropout_ratio = 12 [default = 0.5]; // dropout ratio

  optional uint32 local_size = 13 [default = 5]; // for local response norm
  optional float alpha = 14 [default = 1.]; // for local response norm
  optional float beta = 15 [default = 0.75]; // for local response norm
  optional float k = 22 [default = 1.];

  // For data layers, specify the data source
  optional string source = 16;
  // For data pre-processing, we can do simple scaling and subtracting the
  // data mean, if provided. Note that the mean subtraction is always carried
  // out before scaling.
  optional float scale = 17 [default = 1];
  optional string meanfile = 18;
  // For data layers, specify the batch size.
  optional uint32 batchsize = 19;
  // For data layers, specify if we would like to randomly crop an image.
  optional uint32 cropsize = 20 [default = 0];
  // For data layers, specify if we want to randomly mirror data.
  optional bool mirror = 21 [default = false];

  // The blobs containing the numeric parameters of the layer
  repeated BlobProto blobs = 50;
  // The ratio that is multiplied on the global learning rate. If you want to
  // set the learning ratio for one blob, you need to set it for all blobs.
  repeated float blobs_lr = 51;
  // The weight decay that is multiplied on the global weight decay.
  repeated float weight_decay = 52;

  // The rand_skip variable is for the data layer to skip a few data points
  // to avoid all asynchronous sgd clients to start at the same point. The skip
  // point would be set as rand_skip * rand(0,1). Note that rand_skip should not
  // be larger than the number of keys in the database.
  optional uint32 rand_skip = 53 [default = 0];

  // Fields related to detection (det_*)
  // foreground (object) overlap threshold
  optional float det_fg_threshold = 54 [default = 0.5];
  // background (non-object) overlap threshold
  optional float det_bg_threshold = 55 [default = 0.5];
  // Fraction of batch that should be foreground objects
  optional float det_fg_fraction = 56 [default = 0.25];

  // optional bool OBSOLETE_can_clobber = 57 [default = true];

  // Amount of contextual padding to add around a window
  // (used only by the window_data_layer)
  optional uint32 det_context_pad = 58 [default = 0];

  // Mode for cropping out a detection window
  // warp: cropped window is warped to a fixed size and aspect ratio
  // square: the tightest square around the window is cropped
  optional string det_crop_mode = 59 [default = "warp"];

  // For ReshapeLayer, one needs to specify the new dimensions.
  optional int32 new_num = 60 [default = 0];
  optional int32 new_channels = 61 [default = 0];
  optional int32 new_height = 62 [default = 0];
  optional int32 new_width = 63 [default = 0];

  // Whether or not ImageLayer should shuffle the list of files at every epoch.
  // It will also resize images if new_height or new_width are not zero.
  optional bool shuffle_images = 64 [default = false];

  // For ConcatLayer, one needs to specify the dimension for concatenation, and
  // the other dimensions must be the same for all the bottom blobs.
  // By default it will concatenate blobs along the channels dimension.
  optional uint32 concat_dim = 65 [default = 1];

  optional HDF5OutputParameter hdf5_output_param = 1001;
}

message PReLUParameter {
  // Parametric ReLU described in K. He et al, Delving Deep into Rectifiers:
  // Surpassing Human-Level Performance on ImageNet Classification, 2015.

  // Initial value of a_i. Default is a_i=0.25 for all i.
  optional FillerParameter filler = 1;
  // Whether or not slope parameters are shared across channels.
  optional bool channel_shared = 2 [default = false];
}
