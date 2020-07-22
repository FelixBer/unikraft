/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Author: Tamas K Lengyel <tamas.lengyel@zentific.com>
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "xen_private.h"

static status_t sanity_check(xen_instance_t *xen)
{
    status_t ret = VMI_FAILURE;
    libxc_wrapper_t *w = &xen->libxcw;

    if ( !w->xc_interface_open || !w->xc_interface_close || !w->xc_version ||
            !w->xc_map_foreign_range || !w->xc_vcpu_getcontext || !w->xc_vcpu_setcontext ||
            !w->xc_domain_hvm_getcontext || !w->xc_domain_hvm_getcontext_partial ||
            !w->xc_domain_hvm_setcontext || !w->xc_domain_getinfo || !w->xc_domain_getinfolist ||
            !w->xc_domctl || !w->xc_domain_pause || !w->xc_domain_unpause )
        return ret;

    xen->xchandle = xen->libxcw.xc_interface_open(NULL, NULL, 0);
    if ( !xen->xchandle ) {
        errprint("Failed to open libxc interface.\n");
        return ret;
    }

    if ( xen->major_version != 4 )
        goto done;

    switch ( xen->minor_version ) {
        case 0:
            dbprint(VMI_DEBUG_XEN, "Xen 4.0 no longer supported!\n");
            break;
        case 1:
            if ( !w->xc_domain_maximum_gpfn )
                break;

            ret = VMI_SUCCESS;
            break;
        case 2:
        case 3:
        case 4:
            if ( !w->xc_domain_maximum_gpfn || !w->xc_map_foreign_batch ||
                    !w->xc_mem_access_enable || !w->xc_mem_access_disable ||
                    !w->xc_mem_access_resume || !w->xc_domain_debug_control ||
                    !w->xc_domain_set_access_required || !w->xc_hvm_inject_trap ||
                    !w->xc_domain_decrease_reservation_exact || !w->xc_domain_populate_physmap_exact ||
                    !w->xc_evtchn_open || !w->xc_evtchn_close || !w->xc_evtchn_fd ||
                    !w->xc_evtchn_notify || !w->xc_evtchn_pending || !w->xc_evtchn_unmask ||
                    !w->xc_evtchn_unbind || !w->xc_evtchn_bind_interdomain ||
                    !w->xc_set_hvm_param || !w->xc_get_hvm_param || !w->xc_domain_cacheflush)
                break;

            ret = VMI_SUCCESS;
            break;

        case 5:
            if ( !w->xc_domain_maximum_gpfn || !w->xc_set_mem_access ||
                    !w->xc_get_mem_access || !w->xc_mem_access_enable2 ||
                    !w->xc_mem_access_disable || !w->xc_hvm_param_set ||
                    !w->xc_hvm_param_get || !w->xc_domain_cacheflush)
                break;

            ret = VMI_SUCCESS;
            break;

        default:
        /* Things start to be a bit saner from 4.6 */
        case 11:
            if ( !w->xc_monitor_emul_unimplemented )
                break;
        /* Fall-through */
        case 10:
            if ( !w->xc_monitor_descriptor_access || !w->xc_monitor_write_ctrlreg2 )
                break;
        /* Fall-through */
        case 9:
        /* Fall-through */
        case 8:
            if ( !w->xc_monitor_debug_exceptions || !w->xc_monitor_cpuid ||
                    !w->xc_monitor_mov_to_msr2 || !w->xc_monitor_privileged_call)
                break;
        /* Fall-through */
        case 7:
        /* Fall-through */
        case 6:
            if ( !w->xc_domain_maximum_gpfn2 || !w->xc_set_mem_access ||
                    !w->xc_get_mem_access || !w->xc_monitor_enable ||
                    !w->xc_monitor_disable || !w->xc_monitor_resume ||
                    !w->xc_monitor_get_capabilities || !w->xc_monitor_write_ctrlreg ||
                    !w->xc_monitor_mov_to_msr || !w->xc_monitor_singlestep ||
                    !w->xc_monitor_software_breakpoint || !w->xc_monitor_guest_request ||
                    !w->xc_altp2m_get_domain_state || !w->xc_altp2m_set_domain_state ||
                    !w->xc_altp2m_set_vcpu_enable_notify || !w->xc_altp2m_create_view ||
                    !w->xc_altp2m_destroy_view || !w->xc_altp2m_switch_to_view ||
                    !w->xc_altp2m_set_mem_access || !w->xc_altp2m_change_gfn ||
                    !w->xc_hvm_param_set || !w->xc_hvm_param_get ||
                    !w->xc_domain_cacheflush || !w->xc_domain_set_access_required)
                break;

            ret = VMI_SUCCESS;
            break;
    };

done:
    if ( VMI_FAILURE == ret )
        w->xc_interface_close(xen->xchandle);

    return ret;
}

status_t create_libxc_wrapper(xen_instance_t *xen)
{
    libxc_wrapper_t *wrapper = &xen->libxcw;

    memset(wrapper, 0, sizeof(*wrapper));
    wrapper->handle = (void*)-1;

    /* Basic */
    wrapper->xc_interface_open = xc_interface_open;
    wrapper->xc_interface_close = xc_interface_close;
    wrapper->xc_version = xc_version;
    wrapper->xc_map_foreign_pages = xc_map_foreign_pages;
    wrapper->xc_map_foreign_range = xc_map_foreign_range;
    wrapper->xc_domain_get_tsc_info = xc_domain_get_tsc_info;
    wrapper->xc_vcpu_getcontext = xc_vcpu_getcontext;
    wrapper->xc_vcpu_setcontext = xc_vcpu_setcontext;
    wrapper->xc_domain_hvm_getcontext = xc_domain_hvm_getcontext;
    wrapper->xc_domain_hvm_getcontext_partial = xc_domain_hvm_getcontext_partial;
    wrapper->xc_domain_hvm_setcontext = xc_domain_hvm_setcontext;
    wrapper->xc_domain_getinfo = xc_domain_getinfo;
    wrapper->xc_domain_getinfolist = xc_domain_getinfolist;
    wrapper->xc_domctl = xc_domctl;
    wrapper->xc_domain_pause = xc_domain_pause;
    wrapper->xc_domain_unpause = xc_domain_unpause;
  wrapper->xc_domain_maximum_gpfn = xc_domain_maximum_gpfn;
    wrapper->xc_domain_maximum_gpfn2 = xc_domain_maximum_gpfn;
wrapper->xc_map_foreign_batch = 0;
    wrapper->xc_domain_cacheflush = xc_domain_cacheflush;

    /* Events */
    wrapper->xc_vm_event_get_version = xc_vm_event_get_version;
    wrapper->xc_domain_debug_control = xc_domain_debug_control;
    wrapper->xc_domain_set_access_required = xc_domain_set_access_required;
    wrapper->xc_domain_decrease_reservation_exact = xc_domain_decrease_reservation_exact;
wrapper->xc_hvm_inject_trap = 0;
    wrapper->xc_domain_populate_physmap_exact = xc_domain_populate_physmap_exact;
    wrapper->xc_evtchn_open = xc_evtchn_open;
    wrapper->xc_evtchn_close = xc_evtchn_close;
    wrapper->xc_evtchn_fd = xc_evtchn_fd;
    wrapper->xc_evtchn_notify = xc_evtchn_notify;
    wrapper->xc_evtchn_pending = xc_evtchn_pending;
    wrapper->xc_evtchn_unmask = xc_evtchn_unmask;
    wrapper->xc_evtchn_unbind = xc_evtchn_unbind;
    wrapper->xc_evtchn_bind_interdomain = xc_evtchn_bind_interdomain;
    wrapper->xc_set_mem_access = xc_set_mem_access;
    wrapper->xc_get_mem_access = xc_get_mem_access;
wrapper->xc_mem_access_enable = 0;
    wrapper->xc_mem_access_enable2 = 0; // xc_mem_access_enable;
    wrapper->xc_mem_access_disable = 0; //xc_mem_access_disable;
wrapper->xc_mem_access_resume = 0;
    wrapper->xc_monitor_enable = xc_monitor_enable;
    wrapper->xc_monitor_disable = xc_monitor_disable;
    wrapper->xc_monitor_resume = xc_monitor_resume;
    wrapper->xc_monitor_get_capabilities = xc_monitor_get_capabilities;
  wrapper->xc_monitor_write_ctrlreg = xc_monitor_write_ctrlreg;
    wrapper->xc_monitor_write_ctrlreg2 = xc_monitor_write_ctrlreg;
  wrapper->xc_monitor_mov_to_msr = xc_monitor_mov_to_msr;
    wrapper->xc_monitor_mov_to_msr2 = xc_monitor_mov_to_msr;
    wrapper->xc_monitor_singlestep = xc_monitor_singlestep;
    wrapper->xc_monitor_software_breakpoint = xc_monitor_software_breakpoint;
    wrapper->xc_monitor_guest_request = xc_monitor_guest_request;  // ????????????????????? potential libvmi bug
    wrapper->xc_monitor_privileged_call = xc_monitor_privileged_call;
    wrapper->xc_monitor_descriptor_access = xc_monitor_descriptor_access;
    wrapper->xc_monitor_emul_unimplemented = xc_monitor_emul_unimplemented;
    wrapper->xc_altp2m_get_domain_state = xc_altp2m_get_domain_state;
    wrapper->xc_altp2m_set_domain_state = xc_altp2m_set_domain_state;
    wrapper->xc_altp2m_set_vcpu_enable_notify = xc_altp2m_set_vcpu_enable_notify;
    wrapper->xc_altp2m_create_view = xc_altp2m_create_view;
    wrapper->xc_altp2m_destroy_view = xc_altp2m_destroy_view;
    wrapper->xc_altp2m_switch_to_view = xc_altp2m_switch_to_view;
    wrapper->xc_altp2m_set_mem_access = xc_altp2m_set_mem_access;
    wrapper->xc_altp2m_change_gfn = xc_altp2m_change_gfn;
    wrapper->xc_monitor_debug_exceptions = xc_monitor_debug_exceptions;
    wrapper->xc_monitor_cpuid = xc_monitor_cpuid;
    wrapper->xc_hvm_param_get = xc_hvm_param_get;
    wrapper->xc_hvm_param_set = xc_hvm_param_set;
    wrapper->xc_get_hvm_param = xc_get_hvm_param;
    wrapper->xc_set_hvm_param = xc_set_hvm_param;


    if (VMI_FAILURE == sanity_check(xen)) {
        return VMI_FAILURE;
    }

    return VMI_SUCCESS;
}
