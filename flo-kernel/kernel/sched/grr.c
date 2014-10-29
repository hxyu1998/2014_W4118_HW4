/* new GRR schedule */
#include <linux/sched.h>

static DEFINE_SPINLOCK()

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_grr_entity *grr_entity = &p->grr;
	
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
}

static void yield_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
}

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
}

static void set_curr_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static void switch_to_grr(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
}

static const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.dequeue_task		= dequeue_task_grr,
	.enqueue_task		= enqueue_task_grr,
	.yield_task		= yield_task_grr,
	.check_preempt_curr	= check_preempt_curr_grr,
	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
	.pre_schedule		= pre_schedule_grr,
	.post_schedule		= post_schedule_grr,
#endif

	.set_curr_task		= set_curr_task_grr,
	.task_tick		= task_tick_grr,
	
	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};
