/**
 * frame_view.h - Operations which can be performed on a frame such as
 * pivot, move, copy, paste, scale, etc...
 *
 * The view is independant of the controller. The main purpose of the view is to
 * decouple operations on the model from the UI specific code (such as wxWidgets).
 */

/**
 * Define a view interface which any controller can use to manipulate the model.
 */
class view
{
    virtual void handle_left_down(int x, int y) = 0;
    virtual void handle_left_up(int x, int y) = 0;
};

class frame_view
{
    enum {
        OP_SELECT,
        OP_MOVE,
        OP_PIVOT,
    };

    class stick_operation
    {
    };

    class select_operation : stick_operation
    {
    };

    class move_operation : stick_operation
    {
    };

    class pivot_operation : stick_operation
    {
    };

    void handle_left_down(int x, int y)
    {
        switch(current_op_id) {
            case OP_SELECT:
                int nindex;
                figure* fig;

                // determine if we have selected a figure
                if (get_figure_at_pos(x, y, radius_, fig, nindex)) {
                    // nindex is the node that was clicked
                    // fig is the figure which was selected
                    selected_fig_ = fig;
                    if (fig->is_root_node(nindex)) {
                        current_opt_id = OP_MOVE;   // started a move operation
                        delete current_operation_;
                        current_operation_ = new move_operation(fig, x, y);
                    }
                }
                break;

            case OP_MOVE:
            case OP_PIVOT:
            default:
                // these should be invalid for this event
                assert(false);
                break;
        }
    }

    void handle_left_up(int x, int y);

    void handle_move(int x, int y)
    {
        switch(current_op_id) {
            case OP_SELECT:
                break;
            case OP_MOVE:
                current_operation_->move(x, y);
                break;
            case OP_PIVOT:
                break;
        }
    }

private:
    int current_op_id;
    stick_operation_* current_op_;
    frame* the_frame_;
};

