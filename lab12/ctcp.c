/******************************************************************************
 * ctcp.c
 * ------
 * Implementation of cTCP done here. This is the only file you need to change.
 * Look at the following files for references and useful functions:
 *   - ctcp.h: Headers for this file.
 *   - ctcp_iinked_list.h: Linked list functions for managing a linked list.
 *   - ctcp_sys.h: Connection-related structs and functions, cTCP segment
 *                 definition.
 *   - ctcp_utils.h: Checksum computation, getting the current time.
 *
 *****************************************************************************/

#include "ctcp.h"
#include "ctcp_linked_list.h"
#include "ctcp_sys.h"
#include "ctcp_utils.h"

/**
 * Connection state.
 *
 * Stores per-connection information such as the current sequence number,
 * unacknowledged packets, etc.
 *
 * You should add to this to store other fields you might need.
 */
struct ctcp_state
{
  struct ctcp_state *next;  /* Next in linked list */
  struct ctcp_state **prev; /* Prev in linked list */

  conn_t *conn;            /* Connection object -- needed in order to figure
                              out destination when sending */
  linked_list_t *segments; /* Linked list of segments sent to this connection.
                              It may be useful to have multiple linked lists
                              for unacknowledged segments, segments that
                              haven't been sent, etc. Lab 1 uses the
                              stop-and-wait protocol and therefore does not
                              necessarily need a linked list. You may remove
                              this if this is the case for you */

  /* FIXME: Add other needed fields. */
  uint32_t seqno;
  uint32_t ackno;
  uint16_t len;
  uint32_t flag;
};

/**
 * Linked list of connection states. Go through this in ctcp_timer() to
 * resubmit segments and tear down connections.
 */
static ctcp_state_t *state_list;

/* FIXME: Feel free to add as many helper functions as needed. Don't repeat
          code! Helper functions make the code clearer and cleaner. */

ctcp_state_t *ctcp_init(conn_t *conn, ctcp_config_t *cfg)
{
  /* Connection could not be established. */
  if (conn == NULL)
  {
    return NULL;
  }

  /* Established a connection. Create a new state and update the linked list
     of connection states. */
  ctcp_state_t *state = calloc(sizeof(ctcp_state_t), 1);
  state->next = state_list;
  state->prev = &state_list;
  if (state_list)
    state_list->prev = &state->next;
  state_list = state;

  /* Set fields. */
  state->conn = conn;
  /* FIXME: Do any other initialization here. */

  return state;
}

void ctcp_destroy(ctcp_state_t *state)
{
  /* Update linked list. */
  if (state->next)
    state->next->prev = state->prev;

  *state->prev = state->next;
  conn_remove(state->conn);

  /* FIXME: Do any other cleanup here. */

  free(state);
  end_client();
}

void ctcp_read(ctcp_state_t *state)
{
  /* FIXME */

  char buf[MAX_SEG_DATA_SIZE]; // Read data from STDIN

  ctcp_segment_t *seg;
  seg->seqno = state->seqno;
  seg->ackno = state->ackno;
  seg->window = 1 * MAX_SEG_DATA_SIZE;
  seg->flags = 0;
  // seg->len = 0
  // seg->cksum = cksum();
  // int input = conn_input(state->conn, (char *)buf, MAX_SEG_DATA_SIZE);
  int input = conn_input(state->conn, buf, MAX_SEG_DATA_SIZE);
  seg->cksum = cksum(seg->data, strlen(seg->data));
  
  if (input == -1)
  {
    seg->flags = FIN;
    seg->len = HEADER_LENGTH + strlen(seg->data);
    // Send FIN
    conn_send(state->conn, seg, seg->len);
    seg->seqno += strlen(buf);
    ctcp_destroy(state);
  }

  seg->flags = 0;
  memcpy(seg->data, buf, MAX_SEG_DATA_SIZE);
  conn_send(state->conn, seg, seg->len);
  // seg->seqno += strlen(buf);
  state->next->seqno = seg->seqno + strlen(buf);
}

void ctcp_receive(ctcp_state_t *state, ctcp_segment_t *segment, size_t len)
{
  /* FIXME */
  char buf[MAX_SEG_DATA_SIZE];
  ctcp_segment_t *seg;
  seg->seqno = state->seqno;
  seg->ackno = state->ackno + strlen(segment->data) + 1;
  memcpy(seg->data, buf, MAX_SEG_DATA_SIZE);
  seg->flags = ACK;
  seg->len = HEADER_LENGTH;
  seg->window = 1 * MAX_SEG_DATA_SIZE;
  seg->cksum = cksum(seg->data, strlen(seg->data));
  
  conn_send(state->conn, seg, seg->len);

  size_t buf_space = conn_bufspace(state->conn);
  if (buf_space > len)
  {
    conn_output(state->conn, buf, strlen(buf));

    // segment->flags = ACK;
    // segment->ackno += strlen(buf);
  }
}

void ctcp_output(ctcp_state_t *state)
{
  /* FIXME */
  // size_t buf_space = conn_bufspace(state->conn);
  // if (buf_space > state->len)
  // {
  //   conn_output(state->conn, )
  // }
}

void ctcp_timer()
{
  /* FIXME */
}