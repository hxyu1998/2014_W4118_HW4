/* new GRR schedule */
#include "sched.h"
#include <linux/interrupt.h>

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	trace_printk("entering dequeue...\n");	
	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;

	grr_se = &p->grr;
	grr_rq = &rq->grr;

	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_del_init(&grr_se->run_list);
	--grr_rq->grr_nr_running;
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	dec_nr_running(rq);
	trace_printk("leaving dequeue...\n");
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	trace_printk("entering enqueue...\n");
	/* maybe this can simplified 
	struct sched_grr_entity *grr_se = &p->grr;
	struct task_struct *p = container_of(grr_se, struct task_struct, rt);
	struct rq *rq = task_rq(p);
	struct grr_rq *grr_rq = rq->grr;
	struct list_head *queue = grr_rq->grr_rq_list;
	list_add(&grr_se->run_list, queue);
	grr_rq->nr_running++; 	
	*/

	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;
	struct list_head *queue;

	grr_se = &p->grr;
	grr_rq = &rq->grr;
	queue = &grr_rq->grr_rq_list;

	/*Ethan: I changed list_add to list_add_tail.*/
	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_add_tail(&grr_se->run_list, queue);
	grr_rq->grr_nr_running++;
	raw_spin_unlock(&grr_rq->grr_rq_lock);	
	inc_nr_running(rq);
	trace_printk("leaving enqueue...\n");
}

static void requeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	trace_printk("entering requeue...\n");
	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;
	struct list_head *head;

	grr_se = &p->grr;
	grr_rq = &rq->grr;
	head = &grr_rq->grr_rq_list;

	if (sizeof(grr_rq->grr_nr_running) == 1)
		return;
	
	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_move_tail(&grr_se->run_list, head);
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	trace_printk("leaving requeue...\n");
}

static void yield_task_grr(struct rq *rq)
{
	requeue_task_grr(rq, rq->curr , 0);
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	trace_printk("entering pick_next_task_grr...\n");
	struct sched_grr_entity *grr_se;
	struct task_struct *p;
	struct grr_rq *grr_rq;
	struct list_head *queue;

	grr_rq = &rq->grr;

	if(!grr_rq->grr_nr_running)
		return NULL;

	queue = &grr_rq->grr_rq_list;
	grr_se = list_entry(queue->next, struct sched_grr_entity, run_list);
	return container_of(grr_se, struct task_struct, grr);
	trace_printk("leaving pick_next_task_grr...\n");
}

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_grr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_grr_entity *grr_se;

	grr_se = &p->grr;
	if (--p->grr.time_slice)
		return;

	p->grr.time_slice = GRR_TIMESLICE;

	if (grr_se->run_list.prev != grr_se->run_list.next) {
		requeue_task_grr(rq, p, 0);
		set_tsk_need_resched(p);
		return;
	}
}

/*Ethan: this seems is used for group task schedule.*/
static void set_curr_task_grr(struct rq *rq)
{	
	trace_printk("entering set_curr_task_grr...\n");
	struct task_struct *p;
	
	p = rq->curr;
	p->se.exec_start = rq->clock_task;
	
	trace_printk("leaving set_curr_task_grr...\n");
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

/*Ethan: this seems is used for group task schedule.*/
static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
	trace_printk("entering switched_to_grr...\n");
	struct sched_grr_entity *grr_se;

	grr_se = &p->grr;
	grr_se->time_slice = GRR_TIMESLICE;
 	INIT_LIST_HEAD(&grr_se->run_list);
	trace_printk("leaving switched_to_grr...\n");
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
	trace_printk("entering prio_changed_grr...\n");
	trace_printk("leaving prio_changed_grr...\n");
}

/*Ethan: Will be called by syscall scehd_rr_get_interval*/
static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
	if (t == NULL)
		return -EINVAL;
	return GRR_TIMESLICE;
}

void init_grr_rq(struct grr_rq *grr_rq)
{
	printk("entering init_grr_rq...\n");
	grr_rq->grr_nr_running = 0;
	INIT_LIST_HEAD(&grr_rq->grr_rq_list);
	raw_spin_lock_init(&grr_rq->grr_rq_lock);
	printk("leaving init_grr_rq...\n");
}

/*====================For SMP====================*/
#ifdef CONFIG_SMP

static int load_balance_grr(int this_cpu, struct rq *this_rq,
			struct sched_domain *sd, enum cpu_idle_type idle,
			int *balance)
{
}

static void rebalance_domains_grr(int cpu, enum cpu_idle_type idle)
{
}

static void run_rebalance_domains_grr(struct softirq_action *h)
{
}

void trigger_load_balance_grr(struct rq *rq, int cpu)
{
}

static void rq_online_grr(struct rq *rq)
{
}

static void rq_offline_grr(struct rq *rq)
{
}

static void switched_from_grr(struct rq *rq, struct task_struct *p)
{
}

static void pre_schedule_grr(struct rq *rq, struct task_struct *prev)
{
}

static void post_schedule_grr(struct rq *rq, struc)
{
}

static void task_woken_grr(struct rq *rq, struct task_struct *p)
{
}

static int select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{
	struct rq *rq;
	struct grr_rq = *grr_rq;
	int cpu, idle_cpu;
	long loading, min_loading;

	min_loading = LONG_MAX;
	idle_cpu = -1;
	for_each_online_cpu(cpu) {
		rq = cpu_rq(cpu);
		grr_rq = &rq->grr;
		loading = grr_rq->grr_nr_running;

		if (loading < min_loading) {
			min_loading = loading;
			idle_cpu = cpu;
		} 
	}

	if (idle_cpu == -1)
		return task_cpu(p);
	else
		return idle_cpu;
}

#endif

const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.dequeue_task		= dequeue_task_grr,
	.enqueue_task		= enqueue_task_grr,
	.yield_task		= yield_task_grr,
	.check_preempt_curr	= check_preempt_curr_grr,
	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
	.rq_online		= rq_online_grr,
	.rq_offline		= rq_offline_grr,
	.task_woken		= task_woken_grr,
	.switched_from		= switched_from_grr,
	.pre_schedule		= pre_schedule_grr,
	.post_schedule		= post_schedule_grr,
#endif

	.set_curr_task		= set_curr_task_grr,
	.task_tick		= task_tick_grr,
	
	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};

#ifdef CONFIG_SCHED_DEBUG
extern void print_grr_rq(struct seq_file *m, int cpu, struct grr_rq *grr_rq);

void print_grr_stats(struct seq_file *m, int cpu)
{
	struct rq *rq;
	struct grr_rq *grr_rq;
	rq = cpu_rq(cpu);
	rcu_read_lock();
	for (grr_rq = &rq->grr; grr_rq; grr_rq = NULL) {
		print_grr_rq(m, cpu, grr_rq);
	}
	rcu_read_unlock();
}
#endif

#ifdef CONFIG_SMP
	open_softirq(SCHED_GRR_SOFTIRQ, run_reblance_domains_grr);
#endif
