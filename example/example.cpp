#include <mediapipe.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#define CHECK_MP_RESULT(result) \
    if (!result) \
    { \
        const char* error = mp_get_last_error(); \
        std::cerr << "[MediaPipe] " << error; \
        mp_free_error(error); \
        std::exit(1); \
    }

const mp_hand_landmark CONNECTIONS[][2] = {
        {mp_hand_landmark_wrist,             mp_hand_landmark_thumb_cmc},
        {mp_hand_landmark_thumb_cmc,         mp_hand_landmark_thumb_mcp},
        {mp_hand_landmark_thumb_mcp,         mp_hand_landmark_thumb_ip},
        {mp_hand_landmark_thumb_ip,          mp_hand_landmark_thumb_tip},
        {mp_hand_landmark_wrist,             mp_hand_landmark_index_finger_mcp},
        {mp_hand_landmark_index_finger_mcp,  mp_hand_landmark_index_finger_pip},
        {mp_hand_landmark_index_finger_pip,  mp_hand_landmark_index_finger_dip},
        {mp_hand_landmark_index_finger_dip,  mp_hand_landmark_index_finger_tip},
        {mp_hand_landmark_index_finger_mcp,  mp_hand_landmark_middle_finger_mcp},
        {mp_hand_landmark_middle_finger_mcp, mp_hand_landmark_middle_finger_pip},
        {mp_hand_landmark_middle_finger_pip, mp_hand_landmark_middle_finger_dip},
        {mp_hand_landmark_middle_finger_dip, mp_hand_landmark_middle_finger_tip},
        {mp_hand_landmark_middle_finger_mcp, mp_hand_landmark_ring_finger_mcp},
        {mp_hand_landmark_ring_finger_mcp,   mp_hand_landmark_ring_finger_pip},
        {mp_hand_landmark_ring_finger_pip,   mp_hand_landmark_ring_finger_dip},
        {mp_hand_landmark_ring_finger_dip,   mp_hand_landmark_ring_finger_tip},
        {mp_hand_landmark_ring_finger_mcp,   mp_hand_landmark_pinky_mcp},
        {mp_hand_landmark_wrist,             mp_hand_landmark_pinky_mcp},
        {mp_hand_landmark_pinky_mcp,         mp_hand_landmark_pinky_pip},
        {mp_hand_landmark_pinky_pip,         mp_hand_landmark_pinky_dip},
        {mp_hand_landmark_pinky_dip,         mp_hand_landmark_pinky_tip}
};

void draw_landmarks(cv::Mat frame, mp_multi_face_landmark_list *landmarks) {
    for (int i = 0; i < landmarks->length; i++) {
        const mp_landmark_list &hand = landmarks->elements[i];

        // Draw hand connections as green lines.
        for (const auto &connection: CONNECTIONS) {
            const mp_landmark &p1 = hand.elements[connection[0]];
            const mp_landmark &p2 = hand.elements[connection[1]];
            float x1 = (float) frame.cols * p1.x;
            float y1 = (float) frame.rows * p1.y;
            float x2 = (float) frame.cols * p2.x;
            float y2 = (float) frame.rows * p2.y;

            cv::line(frame, {(int) x1, (int) y1}, {(int) x2, (int) y2}, CV_RGB(0, 255, 0), 2);
        }

        // Draw hand landmarks as red dots.
        for (int j = 0; j < hand.length; j++) {
            const mp_landmark &p = hand.elements[j];
            float x = (float) frame.cols * p.x;
            float y = (float) frame.rows * p.y;
            cv::circle(frame, cv::Point((int) x, (int) y), 4, CV_RGB(255, 0, 0), -1);
        }
    }
}

void draw_rects(cv::Mat &frame, mp_rect_list *rects) {
    for (int i = 0; i < rects->length; i++) {
        const mp_rect &rect = rects->elements[i];

        cv::Point2f center((float) frame.cols * rect.x_center, (float) frame.rows * rect.y_center);
        cv::Point2f size((float) frame.cols * rect.width, (float) frame.rows * rect.height);
        float rotation = (float) rect.rotation * (180.0f / (float) CV_PI);

        // Draw hand bounding boxes as blue rectangles.
        cv::Point2f vertices[4];
        cv::RotatedRect(center, size, rotation).points(vertices);
        for (int j = 0; j < 4; j++) {
            cv::line(frame, vertices[j], vertices[(j + 1) % 4], CV_RGB(0, 0, 255), 2);
        }
    }
}

int main(int argc, char **argv) {
    // Check resource directory argument.
    if (argc < 2) {
        std::cerr << "Missing resource directory argument" << std::endl;
        return 1;
    }

    // Open camera.
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cerr << "Failed to open camera" << std::endl;
        return 1;
    }

    // Set MediaPipe resource directory.
    mp_set_resource_dir(argv[1]);

    // Load the binary graph and specify the input stream name.
    std::string path = std::string(argv[1]) + "/mediapipe/modules/hand_landmark/hand_landmark_tracking_cpu.binarypb";
    mp_instance_builder *builder = mp_create_instance_builder(path.c_str(), "image");

    // Configure the graph with node options and side packets.
    mp_add_option_float(builder, "palmdetectioncpu__TensorsToDetectionsCalculator", "min_score_thresh", 0.6);
    mp_add_option_double(builder, "handlandmarkcpu__ThresholdingCalculator", "threshold", 0.2);
    mp_add_side_packet(builder, "num_hands", mp_create_packet_int(2));
    mp_add_side_packet(builder, "model_complexity", mp_create_packet_int(1));
    mp_add_side_packet(builder, "use_prev_landmarks", mp_create_packet_bool(true));

    // Create an instance from the instance builder.
    mp_instance *instance = mp_create_instance(builder);
    CHECK_MP_RESULT(instance)

    // Create poller for the hand landmarks.
    mp_poller *landmarks_poller = mp_create_poller(instance, "multi_hand_landmarks");
    CHECK_MP_RESULT(landmarks_poller)

    // Create a poller for the hand rectangles.
    mp_poller *rects_poller = mp_create_poller(instance, "hand_rects");
    CHECK_MP_RESULT(rects_poller)

    // Start the graph.
    CHECK_MP_RESULT(mp_start(instance))

    cv::Mat frame;
    while (true) {
        // Acquire the next video frame.
        if (!capture.read(frame)) {
            break;
        }

        // Store the frame data in an image structure.
        mp_image image;
        image.data = frame.data;
        image.width = frame.cols;
        image.height = frame.rows;
        image.format = mp_image_format_srgb;

        // Wrap the image in a packet and process it.
        CHECK_MP_RESULT(mp_process(instance, mp_create_packet_image(image)))

        // Wait until the image has been processed.
        CHECK_MP_RESULT(mp_wait_until_idle(instance))

        // Draw the hand landmarks onto the frame.
        if (mp_get_queue_size(landmarks_poller) > 0) {
            mp_packet *packet = mp_poll_packet(landmarks_poller);
            mp_multi_face_landmark_list *landmarks = mp_get_norm_multi_face_landmarks(packet);
            draw_landmarks(frame, landmarks);
            mp_destroy_multi_face_landmarks(landmarks);
            mp_destroy_packet(packet);
        }

        // Draw the hand rectangles onto the frame.
        if (mp_get_queue_size(rects_poller) > 0) {
            mp_packet *packet = mp_poll_packet(rects_poller);
            mp_rect_list *rects = mp_get_norm_rects(packet);
            draw_rects(frame, rects);
            mp_destroy_rects(rects);
            mp_destroy_packet(packet);
        }

        // Display the frame in a window.
        cv::imshow("MediaPipe", frame);
        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    // Clean up resources.
    cv::destroyAllWindows();
    mp_destroy_poller(rects_poller);
    mp_destroy_poller(landmarks_poller);
    mp_destroy_instance(instance);

    return 0;
}